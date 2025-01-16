#include "terrain.h"
#include "cube.h"
#include <globalinstance.h>
#include <iostream>
#include <stdexcept>
#include <thread>

// some definitions
Terrain::Terrain(OpenGLContext *context)
    : m_chunks()
    , m_generatedTerrain()
    , m_geomCube(context)
    , mp_context(context)
    , m_ChunksByThreads()
    , mp_riverMap(
	  new std::array<std::array<bool, RIVERMAX>, RIVERMAX>) // initialize a brand new river map
    , m_ChunksVBOcreatedByThreads()
{}

Terrain::~Terrain() {
    m_geomCube.destroyVBOdata();
}

// Combine two 32-bit ints into one 64-bit int
// where the upper 32 bits are X and the lower 32 bits are
int64_t toKey(int x, int z)
{
    int64_t xz = 0xffffffffffffffff;
    int64_t x64 = x;
    int64_t z64 = z;

    // Set all lower 32 bits to 1 so we can & with Z later
    xz = (xz & (x64 << 32)) | 0x00000000ffffffff;

    // Set all upper 32 bits to 1 so we can & with XZ
    z64 = z64 | 0xffffffff00000000;

    // Combine
    xz = xz & z64;
    return xz;
}

glm::ivec2 toCoords(int64_t k) {
    // Z is lower 32 bits
    int64_t z = k & 0x00000000ffffffff;
    // If the most significant bit of Z is 1, then it's a negative number
    // so we have to set all the upper 32 bits to 1.
    // Note the 8    V
    if(z & 0x0000000080000000) {
        z = z | 0xffffffff00000000;
    }
    int64_t x = (k >> 32);

    return glm::ivec2(x, z);
}

// Surround calls to this with try-catch if you don't know whether
// the coordinates at x, y, z have a corresponding Chunk
BlockType Terrain::getBlockAt(int x, int y, int z) const
{
    if (hasChunkAt(x, z)) {
	// Just disallow action below or above min/max height,
	// but don't crash the game over it.
	if (y < 0 || y >= 256) {
	  return EMPTY;
	}
	const uPtr<Chunk> &c = getChunkAt(x, z);
	glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
	return c->getBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
			     static_cast<unsigned int>(y),
			     static_cast<unsigned int>(z - chunkOrigin.y));
    } else {
	throw std::out_of_range("Coordinates " + std::to_string(x) + " " + std::to_string(y) + " "
				+ std::to_string(z) + " have no Chunk!");
    }
}

BlockType Terrain::getBlockAt(glm::vec3 p) const {
    return getBlockAt(p.x, p.y, p.z);
}

bool Terrain::hasChunkAt(int x, int z) const {
    // Map x and z to their nearest Chunk corner
    // By flooring x and z, then multiplying by 16,
    // we clamp (x, z) to its nearest Chunk-space corner,
    // then scale back to a world-space location.
    // Note that floor() lets us handle negative numbers
    // correctly, as floor(-1 / 16.f) gives us -1, as
    // opposed to (int)(-1 / 16.f) giving us 0 (incorrect!).
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks.find(toKey(16 * xFloor, 16 * zFloor)) != m_chunks.end();
}


uPtr<Chunk>& Terrain::getChunkAt(int x, int z) {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks[toKey(16 * xFloor, 16 * zFloor)];
}


const uPtr<Chunk>& Terrain::getChunkAt(int x, int z) const {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks.at(toKey(16 * xFloor, 16 * zFloor));
}

void Terrain::setBlockAt(int x, int y, int z, BlockType t)
{
    if(hasChunkAt(x, z)) {
        uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = c->getOrigin();
        c->setBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                      static_cast<unsigned int>(y),
                      static_cast<unsigned int>(z - chunkOrigin.y),
                      t);
        setChunkDirty(c.get());
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
}

Chunk* Terrain::instantiateChunkAt(int x, int z) {
    uPtr<Chunk> chunk = mkU<Chunk>(mp_context, x, z);
    Chunk *cPtr = chunk.get();
    m_chunks[toKey(x, z)] = std::move(chunk);
    uninitChunks.insert(toKey(x, z));
    // Set the neighbor pointers of itself and its neighbors
    if(hasChunkAt(x, z + 16)) {
        auto &chunkNorth = m_chunks[toKey(x, z + 16)];
        setChunkDirty(toKey(x, z + 16));
        cPtr->linkNeighbor(chunkNorth, ZPOS);
    }
    if(hasChunkAt(x, z - 16)) {
        auto &chunkSouth = m_chunks[toKey(x, z - 16)];
        setChunkDirty(toKey(x, z - 16));
        cPtr->linkNeighbor(chunkSouth, ZNEG);
    }
    if(hasChunkAt(x + 16, z)) {
        auto &chunkEast = m_chunks[toKey(x + 16, z)];
        setChunkDirty(toKey(x + 16, z));
        cPtr->linkNeighbor(chunkEast, XPOS);
    }
    if(hasChunkAt(x - 16, z)) {
        auto &chunkWest = m_chunks[toKey(x - 16, z)];
        setChunkDirty(toKey(x - 16, z));
        cPtr->linkNeighbor(chunkWest, XNEG);
    }
    return cPtr;
}

void Terrain::draw(ShaderProgram *shaderProgram)
{
    // using terrain generation zone coordinate (world coord / 64)
    int xOrigin = floor(playerPos.x / 64.f);
    int zOrigin = floor(playerPos.z / 64.f);

    // TODO: add Frustum Culling
    // opaque pass
    for(int z = -renderRadius; z < renderRadius + 1; ++z)
    {
        int j = z + zOrigin;
        for(int x = -renderRadius; x < renderRadius + 1; ++x)
        {
            int i = x + xOrigin;
            // if terrain generation zone loaded
            if(m_generatedTerrain.find(toKey(i,j)) != m_generatedTerrain.end())
            {
                // if in AABB box
                if (MGlobalInstance::Get->isBoxInFrustum(glm::vec3(i * 64, 0, j * 64), glm::vec3(64, 256, 64)))
                    drawTerrainGenZone(shaderProgram, i, j, 0);
            }
        }
    }
}

void Terrain::drawTransp(ShaderProgram *shaderProgram)
{
    // using terrain generation zone coordinate (world coord / 64)
    int xOrigin = floor(playerPos.x / 64.f);
    int zOrigin = floor(playerPos.z / 64.f);

    for(int z = -renderRadius; z < renderRadius + 1; ++z)
    {
        int j = z + zOrigin;
        for(int x = -renderRadius; x < renderRadius + 1; ++x)
        {
            int i = x + xOrigin;
            // if terrain generation zone loaded
            if(m_generatedTerrain.find(toKey(i,j)) != m_generatedTerrain.end())
            {
                if (MGlobalInstance::Get->isBoxInFrustum(glm::vec3(i * 64, 0, j * 64), glm::vec3(64, 256, 64)))
                    drawTerrainGenZone(shaderProgram, i, j, 1);
            }
        }
    }
}

void Terrain::drawShadow(ShaderProgram *shaderProgram)
{
    // using terrain generation zone coordinate (world coord / 64)
    int xOrigin = floor(playerPos.x / 64.f);
    int zOrigin = floor(playerPos.z / 64.f);

    for(int z = -renderRadius; z < renderRadius + 1; ++z)
    {
        int j = z + zOrigin;
        for(int x = -renderRadius; x < renderRadius + 1; ++x)
        {
            int i = x + xOrigin;
            // if terrain generation zone loaded
            if(m_generatedTerrain.find(toKey(i,j)) != m_generatedTerrain.end())
            {
                drawTerrainGenZone(shaderProgram, i, j, 0);
            }
        }
    }
}

/**
 * @brief Terrain::CreateTestScene create the initial scenes
 */
void Terrain::CreateTestScene()
{
    m_geomCube.createVBOdata();

    // Create the Chunks that will
    // store the blocks for our
    // initial world space crfeatea

    TerrainGenerator::sf_generatorInit();
    // for(int x = 0; x < 64; x += 16) {
    // for(int z = 0; z < 64; z += 16) {
    // Chunk* c = instantiateChunkAt(x, z);
    // m_generatedTerrain.insert(toKey(x / 64, z / 64));
    // uninitChunks.erase(c->getKey());
    // sf_createTerrainChunk(c); // set the individual block type in this step
    // for (int i = 2; i < 16; ++i)
    // {
    // for (int j = 2; j < 16; ++j)
    // {
    // for (int k = 100; k < 150; ++k)
    // {
    // if (i == 2 || i == 15)
    // {
    // c->setBlockAt(i,k,j,OAKLOG);
    // }
    // if (j == 2 || j == 15)
    // {
    // c->setBlockAt(i,k,j,OAKLOG);
    // }
    // if (k == 100 || k == 149)
    // {
    // c->setBlockAt(i,k,j,OAKLOG);
    // }
    // if (c->getBlockAt(i,k-1,j) == GRASS &&
    // c->getBlockAt(i,k+1,j) == EMPTY)
    // {
    // c->setBlockAt(i,k,j,POPPY);
    // }
    // }
    // }
    // }

    // c->createVBOdata();      // create the VBO data for the blocks that have been created
    // // c->setBlockAt(0, 150, 0, LAVA);
    // // c->setBlockAt(0, 143, 0, WATER);
    // }
    // }
    postUpdate();
}

void Terrain::CreateVLTestScene(glm::vec3 pos)
{
    Chunk* chunk = getChunkAt(pos.x, pos.z).get();
    for (int z = 0; z < 16; ++z)
    for (int x = 0; x < 16; ++x)
    {{
        int y = 160;
        for (; y > 100; --y)
        {
            BlockType t = chunk->getBlockAt(x,y,z);
            if (t == GRASS || t == DIRT || t == SAND) break;
            chunk->setBlockAt(x,y,z,EMPTY);
        }
        chunk->setBlockAt(x,y,z,GRASS);
        y++;
        chunk->setBlockAt(x,y,z,POPPY);
        if ((x == 2 || x == 14) || (z == 2 || z == 14))
        {
            for (int i = 0; i < 10; ++i)
            {
                chunk->setBlockAt(x,y+i,z,OAKLOG);
            }
        }
        chunk->setBlockAt(x,y+10,z,OAKLOG);
    }}
    setChunkDirty(chunk);
}

void Terrain::CreateLakeSword(glm::vec3 pos)
{
    int y = pos.y + 1;
    glm::vec3 center;
    for (; y > pos.y - 30; --y)
    {
        BlockType t = getBlockAt(pos.x,y,pos.z);
        if (t == GRASS || t == DIRT || t == SAND)
        {
            center = glm::vec3(pos.x,y,pos.z);
            break;
        }
    }

    for (int z = pos.z - 50; z < pos.z + 51; ++z)
    for (int x = pos.x - 50; x < pos.x + 51; ++x)
    {{
        int y = pos.y + 1;
        for (; y > pos.y - 30; --y)
        {
            BlockType t = getBlockAt(x,y,z);
            if (t == GRASS || t == DIRT || t == SAND) break;
            setBlockAt(x,y,z,EMPTY);
        }

        glm::vec3 p = glm::vec3(x,y,z);
        float length = glm::length(glm::vec2(p.x - center.x, p.z - center.z));
        if (length < 16.f)
        {
            float h = 4.f * (1.0 - glm::smoothstep(0.75f, 1.f, length / 16.f));
            for (int i = 0; i < h; ++i)
            {
                setBlockAt(x, p.y + i, z, DIRT);
            }
            setBlockAt(x, p.y + h, z, GRASS);
            if (x >= center.x && TerrainGenerator::rand1D01() > 0.6f)
                setBlockAt(x, p.y + h + 1, z, POPPY);
            else if (x <= center.x && TerrainGenerator::rand1D01() > 0.6f)
                setBlockAt(x, p.y + h + 1, z, DANDELION);
        }
        else if (length < 50.f)
        {
            float a = (length - 16.f) / 34.f;
            float h = 7.f * sin(a * M_PI);
            for (int i = 0; i < h + 10.f; ++i)
            {
                if (p.y + 10.f - i < center.y + 1.f)
                    setBlockAt(x, p.y + 10 - i, z, WATER);
                else
                    setBlockAt(x, p.y + 10 - i, z, EMPTY);
            }
            setBlockAt(x, p.y - h, z, GRASS);
        }

    }}

    for (int y = center.y; y < center.y + 60; ++y){
    for (int x = center.x - 9; x < center.x + 10; ++x){
    for (int z = center.z - 1; z < center.z + 1; ++z){
        float absX = abs(x - (int)center.x);
        if (absX > 1 && absX < 2)
            setBlockAt(x, y, z, STONE);
        else if (absX >= 2)
        {
            BlockType s[3] = {STONE, COBBLE, MOSSYCOBBLE};
            float absZ = abs(z - (int)center.z);
            if (absZ == 1 && absX <= 4)
                setBlockAt(x, y, z, s[(int)(3 * TerrainGenerator::rand1D01())]);
            else if (absZ == 0 && absX <= 9)
                setBlockAt(x, y, z, s[(int)(3 * TerrainGenerator::rand1D01())]);
        }
    }}}

    for (int y = center.y + 59; y < center.y + 63; ++y){
    int xr = 18 - (y - center.y - 59);
    for (int x = center.x - xr; x < center.x + xr + 1; ++x){
    for (int z = center.z - 5; z < center.z + 6; ++z){
        setBlockAt(x, y, z, OBSIDIAN);
    }}}

    DrawBall(glm::vec3(center.x, center.y + 70, center.z), 10.0f, glm::vec3(0.3, 1, 0.3), OAKLOG);
    DrawBall(glm::vec3(center.x, center.y + 82, center.z), 5.0f, glm::vec3(1), GOLDBLOCK);

}

void Terrain::DrawBall(glm::vec3 center, float radius, glm::vec3 scale, BlockType t)
{
    int r = (int)radius;
    for (int y = center.y - r; y < center.y + r + 1; ++y){
    for (int x = center.x - r; x < center.x + r + 1; ++x){
    for (int z = center.z - r; z < center.z + r + 1; ++z){
        float len = glm::length((glm::vec3(x,y,z) - center) / scale);
        if (len < radius)
            setBlockAt(x, y, z, t);
    }}}
}

void Terrain::updatePlayerPos(glm::vec3 pos)
{
    playerPos = pos;
}

void Terrain::threadCreateChunk(Chunk* ptr, ThreadSafeSet<int64_t>& result){
    MGlobalInstance::Get->startProfiler("\t\t\tCreateChunk");

    sf_createTerrainChunk(ptr);
    result.insert(ptr->getKey());

    MGlobalInstance::Get->printProfiler("\t\t\tCreateChunk");
}

void Terrain::threadChunkGenerateVBO(Chunk *ptr, ThreadSafeSet<Chunk *> & result)
{
    ptr->createVBOdata();
    result.insert(ptr);
}

void Terrain::postUpdate()
{
    MGlobalInstance::Get->startProfiler("\t\tTerrainPostUpdate");


    int toProcessInThisFrame = 5;
    if (uninitChunks.size() > 0)
    {
        for (auto i = uninitChunks.begin(); i != uninitChunks.end();)
        {
            auto key = *i;
            i = uninitChunks.erase(i);
            chunkInitStart(key);
            MGlobalInstance::Get->ThreadPool.thread(threadCreateChunk,
                                                    m_chunks[key].get(),
                                                    std::ref(m_ChunksByThreads));
#if 0
            new std::thread(mtCreateChunk,
                            m_chunks[key].get(),
                            key, std::ref(m_ChunksByThreads));
#endif

            --toProcessInThisFrame;
            if (toProcessInThisFrame == 0) break;
        }
    }
    else{
        for (auto i = dirtyChunksReadyBuild.begin(); i != dirtyChunksReadyBuild.end();)
        {
            auto key = *i;
            ++i;

            chunkVBOBuildStart(key);
            MGlobalInstance::Get->ThreadPool.thread(threadChunkGenerateVBO,
                                                    m_chunks[key].get(),
                                                    std::ref(m_ChunksVBOcreatedByThreads));
#if 0
            new std::thread(mtChunkGenerateVBO,
                            m_chunks[key].get(),
                            std::ref(m_ChunksVBOcreatedByThreads));
#endif
            --toProcessInThisFrame;
            if (toProcessInThisFrame == 0) break;
        }
    }

    joinThreadResult();

    MGlobalInstance::Get->printProfiler("\t\tTerrainPostUpdate");
}

void Terrain::loadNewChunkFromPlayer()
{
    // using terrain generation zone coordinate (world coord / 64)
    int xOrigin = floor(playerPos.x / 64.f);
    int zOrigin = floor(playerPos.z / 64.f);

    for(int z = -loadRadius; z < loadRadius + 1; ++z)
    {
        int j = z + zOrigin;
        for(int x = -loadRadius; x < loadRadius + 1; ++x)
        {
            int i = x + xOrigin;
            // if terrain generation zone not loaded
            if(m_generatedTerrain.find(toKey(i,j)) == m_generatedTerrain.end())
            {
                loadTerrainGenZone(i, j);
                m_generatedTerrain.insert(toKey(i,j));
            }
        }
    }
}

void Terrain::loadTerrainGenZone(int x, int z) 
{
    int x_w = x * 64;
    int z_w = z * 64;

    for(int i = 0; i < 64; i += 16)
    {
        for(int j = 0; j < 64; j += 16)
        {
            int x_c = x_w + i;
            int z_c = z_w + j;
            instantiateChunkAt(x_c, z_c);
        }
    }
}

void Terrain::drawTerrainGenZone(ShaderProgram *shaderProgram, int x, int z, int mode)
{
    int x_w = x * 64;
    int z_w = z * 64;
    for (int i = 0; i < 64; i += 16) {
	for (int j = 0; j < 64; j += 16) {
	    int x_c = x_w + i;
	    int z_c = z_w + j;
	    Chunk *chunk = getChunkAt(x_c, z_c).get();

	    if (mode == 2) {
		shaderProgram->draw(*chunk);
		continue;
	    }

	    /// DO NOT DRAW if the chunk is outside the frustum
	    if (!MGlobalInstance::Get->isChunkInFrustum(chunk)) {
		continue;
	    }

	    if (chunk != nullptr) {
		if (mode == 0)
		    shaderProgram->draw(*chunk);
		else if (mode == 1)
		    shaderProgram->drawTransp(*chunk);
	    }
	}
    }
}

bool Terrain::isChunkIniting(int64_t k)
{
    return m_initingChunk.find(k) != m_initingChunk.end();
}

void Terrain::chunkInitStart(int64_t k)
{
    m_initingChunk.insert(k);
}

void Terrain::chunkInitComplete(int64_t k)
{
    m_initingChunk.erase(k);
}

void Terrain::playerPlaceBlock(int x, int y, int z, BlockType t)
{
    setBlockAt(x, y, z, t);
}

void Terrain::playerBreakBlock(int x, int y, int z)
{
    setBlockAt(x, y, z, EMPTY);
}

void Terrain::RegisterTerrainTaskCompleteAction(std::function<void ()> func)
{
    m_onTerrainReady = func;
}

void Terrain::UnregisterTerrainTaskCompleteAction()
{
    m_onTerrainReady = nullptr;
}

// draw trees
void Terrain::joinThreadResult()
{
    m_ChunksByThreads.lock(true);
    if(!m_ChunksByThreads.getUnsafe.empty()){
        for(auto id : m_ChunksByThreads.getUnsafe){
            chunkInitComplete(id);
            setChunkDirty(id);
        }
        m_ChunksByThreads.getUnsafe.clear();
    }
    m_ChunksByThreads.lock(false);

    m_ChunksVBOcreatedByThreads.lock(true);
    if(!m_ChunksVBOcreatedByThreads.getUnsafe.empty()){
        for(auto chunk : m_ChunksVBOcreatedByThreads.getUnsafe){
            // If the chunk is still dirty,
            // there must be other operation changes the chunk during its VBO generatioin
            // Hence no need to bind this generation
            if(!isChunkDirty(chunk)){
                chunk->bindVBOdata();
            }
            chunkVBOBuildComplete(chunk);   // Calls the end of VBO generation for this chunk
        }
        m_ChunksVBOcreatedByThreads.getUnsafe.clear();
    }
    m_ChunksVBOcreatedByThreads.lock(false);


    if(isAllTaskComplete()){
        allTerrainTaskComplete();
    }
}

/**
 * @brief Terrain::sf_generateConditionPattern
 * @param pattern
 */
void Terrain::sf_generateConditionPattern(std::vector<E_AINPUT> &pattern,
					  const int &x,
					  const int &y,
					  const int &z)
{
    if (0 <= y && y < 60) {
	pattern.emplace_back(HEIGHT0_60);
    }

    if (60 <= y && y < 120) {
	pattern.emplace_back(HEIGHT60_120);
    }

    if (120 <= y && y < 128) {
	pattern.emplace_back(HEIGHT121_128);
    }

    if (128 <= y && y < 160) {
	pattern.emplace_back(HEIGHT_128_160);
    }

    if (160 <= y && y < 250) {
	pattern.emplace_back(HEIGHT121_128);
    }
}

/**
 * @brief Based on the current height generated by the noise function,
 *        return the corresponding block type.
 *
 * @param curr_height | height of the current block
 * @param terrain_height | maximum height of the terrain at the current world positionb
 * @param  isTop whether the block is on the top of the terrain
 * @return BlockType according to the height, if isTop is true, return the top block type
 */
BlockType Terrain::getBlocktype(
    int x, int z, int curr_height, int terrain_height, bool ifTop, int offset)
{
    BlockType BlockState = EMPTY;
    int riverMinHeight = 120;
    int riverMaxHeight = 125;
    // seperate River Logic
    // Need to draw the river before all the terrain generation
    if (terrain_height >= 180) { // intentionally set so there's no river flowing
	riverMaxHeight = terrain_height;
	riverMinHeight = terrain_height;
    } else if (terrain_height >= 160) {
	riverMaxHeight = terrain_height - 2;
	riverMinHeight = terrain_height - 4;
    } else if (terrain_height == 140) {
	riverMaxHeight = 139;
	riverMinHeight = 120;
    } else if (terrain_height > 140) {
	riverMaxHeight = terrain_height - 3;
	riverMinHeight = terrain_height - 5;
    } else {
	riverMaxHeight = 125;
	riverMinHeight = 120;
    }

#ifdef BUILD_RIVER
    // given a world coord, remap the coordinate to [RIVERMIN, RIVERMAX]
    if (z <= RIVERMAX - 1 && z >= RIVERMIN) {
	if (x >= RIVERMIN) { // x>= 0
	    int remappedX = x % RIVERMAX;
	    remappedX = (x / RIVERMAX) % 2 == 1 ? RIVERMAX - 1 - remappedX : remappedX;
	    if ((*TerrainGenerator::sp_riverMap)[remappedX][z]) { // if River is True
		if ((curr_height >= riverMinHeight && curr_height <= riverMaxHeight)) {
		    return WATER;
		}
		if (curr_height > riverMaxHeight) {
		    return EMPTY;
		}
	    }
	} else {
	    int remappedX = abs(x) % RIVERMAX;
	    remappedX = (abs(x) / RIVERMAX) % 2 == 1 ? RIVERMAX - 1 - remappedX : remappedX;
	    if ((*TerrainGenerator::sp_riverMap2)[remappedX][z]) { // if River is True
		if ((curr_height >= riverMinHeight && curr_height <= riverMaxHeight)) {
		    return WATER;
		}
		if (curr_height > riverMaxHeight) {
		    return EMPTY;
		}
	    }
	}
    }
#endif

    bool isEmpty = TerrainGenerator::isHole(x, z, curr_height);
    if (!isEmpty) { // not empty terrains by default
	if (curr_height <= terrain_height) {
	    if (curr_height < 122) {
		return STONE;
	    } else if (curr_height < 128) {
		return SAND;
	    } else if (curr_height < 160 + offset) {
		return ifTop ? GRASS : DIRT;
	    } else if (curr_height < 250) {
		if (curr_height < 200 + offset) {
		    return STONE;
		} else {
		    return ifTop ? SNOW : STONE;
		}
	    } else {
		return EMPTY;
	    }
	} else {
	    if (curr_height == 120) {
		return WATER;
	    }
	    return EMPTY;
	}
    } else {
	if (curr_height >= 120) {
	    if (curr_height <= terrain_height) {
		if (curr_height < 122) {
		    return STONE;
		} else if (curr_height < 128) {
		    return SAND;
		} else if (curr_height < 160 + offset) {
		    return ifTop ? GRASS : DIRT;
		} else if (curr_height < 250) {
		    if (curr_height < 200 + offset) {
		      return STONE;
		    } else {
		      return ifTop ? SNOW : STONE;
		    }
		} else {
		    return EMPTY;
		}
	    } else {
		if (curr_height == 120) {
		    return WATER;
		}
		return EMPTY;
	    }
	} else { // where the hole system will proceed to work
	    if (curr_height >= 120 && curr_height <= 128) {
		return EMPTY;
	    } else if (curr_height > 60 && curr_height < 100) {
		return LAVA;
	    } else {
		return EMPTY;
	    }
	}
    }
}

/**
 * @brief Based on the current Chunk world Pos (lower-left corner pos)
 *        set the Block according to the noise function generated height value
 * @param chunk | pointer to the current 16x16 sized chunk
 */
void Terrain::sf_createTerrainChunk(Chunk *chunk)
{
    glm::ivec2 worldPos = chunk->getOrigin();

    for (int i = 0; i < 16; i++) {
	for (int j = 0; j < 16; j++) {
	    int worldX = worldPos[0] + i;
	    int worldZ = worldPos[1] + j;
	    int terrainHeight = TerrainGenerator::getHeight(
		worldX, worldZ); // maximum height of the current world x, world z
	    int offset = TerrainGenerator::intervalRemapper(
		TerrainGenerator::fbm2D(worldX, worldZ, 0.82, NORMAL, 1), 0.f, 1.f, -4.f, 4.f);
	    for (int k = 0; k <= MAX_HEIGHT; k++) {
		chunk->setBlockAt(i,
				  k,
				  j,
				  getBlocktype(worldX,
					       worldZ,
					       k,
					       terrainHeight,
					       k == terrainHeight,
					       offset));
	    }
	}
    }

#ifdef BUILD_GIANT_TREE
    // Plant Tree At ()
    int worldOriginX = 64;
    int worldOriginZ = 64;
    int worldOriginY = TerrainGenerator::getHeight(64, 64) - 10;

    // generate a tree along the X-Y plane
    int diffX = 64 - worldOriginX;
    int diffY = 0 - worldOriginY;
    int diffZ = 64 - worldOriginZ;
    // define a border box that confines the world Coordinate system
    // any world Coord that falls into this box will be tested
    // for tree generation
    const int MinX = worldOriginX - 64;
    const int MaxX = worldOriginX + 63;
    const int MinZ = worldOriginZ - 64;
    const int MaxZ = worldOriginZ + 63;
    const int MinY = worldOriginY;
    const int MaxY = worldOriginY + 127;

    // For Each Block
    for (int i = 0; i < 16; i++) {
	for (int j = 0; j < 16; j++) {
	    int WorldX = worldPos[0] + i;
	    int WorldZ = worldPos[1] + j;
	    for (int k = MinY; k <= MaxY - 20; k++) {
		if (!(WorldX >= MinX && WorldX <= MaxX && WorldZ >= worldOriginZ - 5
		      && WorldZ <= worldOriginZ + 5)) {
		    continue;
		}

		float value = TerrainGenerator::intervalRemapper(
		    TerrainGenerator::PerlinNoise3D((WorldX + diffX) * 128,
						    k * 128,
						    (WorldZ + diffZ) * 96),
		    -1.f,
		    1.f,
		    0.f,
		    1.f);
		// convert world coord to indexing
		if ((*TerrainGenerator::sp_treeMap)[k + diffY][WorldX + diffX] && value > 0.492) {
		    chunk->setBlockAt(i, k, j, OAKLOG);
		}
	    }
	    for (int k = MinY; k <= MaxY - 20; k++) {
		if (!(WorldZ >= MinZ && WorldZ <= MaxZ && WorldX >= worldOriginX - 5
		      && WorldX <= worldOriginX + 5)) {
		    continue;
		}

		float value = TerrainGenerator::intervalRemapper(
		    TerrainGenerator::PerlinNoise3D((WorldX + diffX) * 128,
						    k * 64,
						    (WorldZ + diffZ) * 96),
		    -1.f,
		    1.f,
		    0.f,
		    1.f);
		// convert world coord to indexing
		if ((*TerrainGenerator::sp_treeMap)[k + diffY][WorldZ + diffZ] && value > 0.4) {
		    chunk->setBlockAt(i, k, j, OAKLOG);
		}
	    }
	    // Draw Grand Tree leaf
	    int currMaxZ, currMaxX, currMinZ, currMinX = 0;
	    int halfHeight = (MaxY - MinY - 20) / 2;
	    for (int k = MinY + halfHeight - 32; k <= MaxY - 20; k++) {
		int currHeight = k - MinY;
		currMaxZ = worldOriginZ + currHeight / 2 + 10;
		currMaxX = worldOriginX + currHeight / 2 + 10;
		currMinZ = worldOriginZ - currHeight / 2 - 10;
		currMinX = worldOriginX - currHeight / 2 - 10;

		if (!(WorldZ >= currMinZ && WorldZ <= currMaxZ && WorldX >= currMinX
		      && WorldX <= currMaxX)) {
		    continue;
		}
		float value1 = TerrainGenerator::intervalRemapper(
		    TerrainGenerator::PerlinNoise3D((WorldX + diffX) * 128,
						    k * 64,
						    (WorldZ + diffZ) * 96),
		    -1.f,
		    1.f,
		    0.f,
		    1.f);
		float value2 = TerrainGenerator::intervalRemapper(
		    TerrainGenerator::PerlinNoise3D((WorldX) / 8, k / 16, (WorldZ) / 8),
		    -1.f,
		    1.f,
		    0.f,
		    1.f);

		float value3 = TerrainGenerator::intervalRemapper(
		    TerrainGenerator::PerlinNoise3D(WorldX, k, WorldZ / 8),
		    -1.f,
		    1.f,
		    0.f,
		    1.f);
		if (value1 > 0.8 && value2 > 0.5) {
		    if (value3 > 0.3) {
		      chunk->setBlockAt(i, k, j, OAKLEAF);
		    } else {
		    chunk->setBlockAt(i, k, j, DANDELION);
		    }
		}
	    }
	}
    }

#endif
    // Draw Grand Tree Leaves
    

#ifdef BUILD_L_TREE
	// draw Normal Trees
    for (int i = 0; i < 16; i++) {
	for (int j = 0; j < 16; j++) {
	    int worldX = worldPos[0] + i;
	    int worldZ = worldPos[1] + j;
	    int terrainHeight = TerrainGenerator::getHeight(worldX, worldZ);
        sf_PlantBTree(i, j, worldX, worldZ, terrainHeight, chunk);
	}
    }
#endif


#ifdef BUILD_GRASS

    // draw grass
    for(int i = 0; i < 16; i++){ for(int j = 0; j < 16; j++){
            int worldX = worldPos[0] + i;
            int worldZ = worldPos[1] + j;
            int terrainHeight = TerrainGenerator::getHeight(worldX, worldZ);
            BlockType soilUnder = chunk->getBlockAt(i, terrainHeight - 1, j);
            if(terrainHeight > 160 || !(soilUnder == DIRT || soilUnder == STONE || soilUnder == GRASS)){
                continue;
            }
            float perturbedValue = TerrainGenerator::SimplexNoise2D(worldX, worldZ);
            float value = TerrainGenerator::intervalRemapper(TerrainGenerator::SimplexNoise2D((worldX + perturbedValue)/ 24, (worldZ + perturbedValue)/24), -1.f, 1.f, 0.f, 1.f);
            if(value > 0.42 && TerrainGenerator::noise2D(i,j) > 0.2){
                chunk->setBlockAt(i, terrainHeight + 1, j, WILDGRASS);
            }
        }
    }

#endif


#ifdef BUILD_FLOWER
    // draw flower
    
    for(int i = 0; i < 16; i++){
        for(int j = 0; j < 16; j++){
            int worldX = worldPos[0] + i;
            int worldZ = worldPos[1] + j;
            int terrainHeight = TerrainGenerator::getHeight(worldX, worldZ);
	    BlockType soilUnder = chunk->getBlockAt(i, terrainHeight - 1, j);
	    if (terrainHeight > 160 + TerrainGenerator::random1(-2, 2)
		|| !(soilUnder == DIRT || soilUnder == STONE || soilUnder == GRASS)) {
		continue;
	    }
	    int perturbedValue = TerrainGenerator::SimplexNoise2D(worldX, worldZ);
            float value = TerrainGenerator::fbm2D(worldX + perturbedValue, worldZ + perturbedValue, 0.83, NORMAL, 9);
	    float isRed = TerrainGenerator::random1(0.f, 1.f);
	    if (value > 0.42) {
		if (isRed > 0.5) {
		    chunk->setBlockAt(i, terrainHeight + 1, j, POPPY);
		} else {
		    chunk->setBlockAt(i, terrainHeight + 1, j, DANDELION);
		}
	    }
	}
    }

#endif

#ifdef BUILD_LAKE

    // draw great lakes
    for (int i = 0; i < 16; i++) {
	for (int j = 0; j < 16; j++) {
	    int worldX = worldPos[0] + i;
	    int worldZ = worldPos[1] + j;
	    // create greate lakes
	    int terrainHeight = TerrainGenerator::getHeight(worldX, worldZ);
	    BlockType blockUnder = chunk->getBlockAt(i, 125, j);
	    for (int k = terrainHeight; k <= 126 && blockUnder != WATER; k++) {
		if (k > terrainHeight) { //
		    chunk->setBlockAt(i, k, j, WATER);
		}
	    }
	}
    }
#endif

}

/**
 * @brief Terrain::isTree
 * @param worldX
 * @param worldY
 * @param worldZ
 * @return 
 */
bool Terrain::isTree(int worldX, int worldZ)
{
    // #if 0
    float value = TerrainGenerator::intervalRemapper(TerrainGenerator::SimplexNoise2D(worldX,
										      worldZ),
						     -1.f,
						     1.f,
						     0.f,
						     1.f);
    if (value > 0.98) {
	return true;
    }

    return false;
    // #endif

#if 0
    if (worldX == -64.f, worldZ == -64.f) {
	return true;
    }
    return false;
#endif
}

bool Terrain::isBTree(int worldX, int worldZ)
{
    float value = glm::fract(sin(12.9898 * worldX + 78.233 * worldZ) * 43758.5453);
    if (value > 0.85) {
    return true;
    }

    return false;
}

/**
 * @brief Draw an real-time generated L-system tree in the Terrain, used
 * to mimic trees that are naturally growing in the low altitude zone
 * 
 * @param localX
 * @param localZ
 * @param worldX
 * @param curr_height
 * @param worldZ
 * @param terrain_height
 * @param chunk
 */
void Terrain::sf_PlantLTree(const int &localX,
			    const int &localZ,
			    const int &worldX,
			    const int &worldZ,
			    const int &terrain_height,
			    Chunk *chunk)
{


}

/**
 * @brief draw a pre-defined tree pattern in the world
 * @param worldX
 * @param curr_height
 * @param WorldZ
 * @param terrain_height
 * @param chunk
 * @return 
 */
void Terrain::sf_PlantNTree(const int &localX,
			    const int &localZ,
			    const int &worldX,
			    const int &worldZ,
			    const int &terrain_height,
			    Chunk *chunk)
{
    const int trunk_height = terrain_height > 140 ? 3 : 3 + TerrainGenerator::random1(0.f, 8.f);
    const int center_ring_height = terrain_height > 140 ? 1 : 1 + TerrainGenerator::random1(0.f, 8.f); // from Trunk to Top
    const int outer_ring_height = 1 + TerrainGenerator::random1(0.f, 1.f); // from Trunk to Top

    // check height and distribution constraint
    if (terrain_height < 128 || terrain_height > 160 || !isTree(worldX / 2, worldZ/ 2)) {
	return;
    }

    // check if the soil is suitable for planting tree
    BlockType soilTypeUnder = chunk->getBlockAt(localX, terrain_height - 1, localZ);
    if (!(soilTypeUnder == STONE || soilTypeUnder == GRASS || soilTypeUnder == DIRT)) {
	return;
    }

    // check near-by block constraint
    for(int i = worldX - 5; i <= worldX + 5; i++){
        for(int j = worldZ - 5; j <= worldZ + 5; j++){
            if(isTree(i, j) && i != worldX && j != worldZ){
                return;
            }
        }
    }

    // catch the unusual mistakes
    if (localX - 2 < 0 || localX + 2 > 15 || localZ - 2 < 0 || localZ + 2 > 15) {
        return;
    }

    // tree trunk
    for (int y = terrain_height + 1; y <= terrain_height + trunk_height; y++) {
        chunk->setBlockAt(localX, y, localZ, OAKLOG);
    }

    // tree ring (center)
    for (int y = terrain_height + trunk_height; y <= terrain_height + 1 + trunk_height + center_ring_height;
	 y++) {
	for (int i = localX - 1; i <= localX + 1; i++){
	    for (int j = localZ - 1 ; j <= localZ + 1; j++) {
		float value = TerrainGenerator::random1(0.f, 1.f);
		if(i == localX && j == localZ){
			chunk->setBlockAt(i, y, j, OAKLEAF);
		}
		if(value > 0.7){
			chunk->setBlockAt(i, y, j, OAKLEAF);
		}
	    }
	}
   }

    // draw the top
    chunk->setBlockAt(localX,
		      terrain_height + 2 + trunk_height + center_ring_height,
		      localZ,
		      OAKLEAF);
}

void Terrain::sf_PlantBTree(const int &localX,
                            const int &localZ,
                            const int &worldX,
                            const int &worldZ,
                            const int &terrain_height,
                            Chunk *chunk)
{
    const int trunk_height = terrain_height > 140 ? 6 : 6 + TerrainGenerator::random1(0.f, 8.f);
    const int center_ring_height = terrain_height > 140 ? 1 : 1 + TerrainGenerator::random1(0.f, 2.f); // from Trunk to Top
    const int outer_ring_height = 1 + TerrainGenerator::random1(0.f, 1.f); // from Trunk to Top

    // check height and distribution constraint
    if (terrain_height < 128 || terrain_height > 160 || !isBTree(worldX, worldZ)) {
    return;
    }

    // check if the soil is suitable for planting tree
    BlockType soilTypeUnder = chunk->getBlockAt(localX, terrain_height - 1, localZ);
    if (!(soilTypeUnder == STONE || soilTypeUnder == GRASS || soilTypeUnder == DIRT)) {
    return;
    }

    // check near-by block constraint
    for(int i = worldX - 2; i <= worldX + 2; i++){
    for(int j = worldZ - 2; j <= worldZ + 2; j++){
        if(isBTree(i , j) && i != worldX && j != worldZ){
        return;
        }
    }
    }

    // tree trunk
    for (int y = terrain_height + 1; y <= terrain_height + trunk_height; y++) {
    chunk->setBlockAt(localX, y, localZ, BIRCHLOG);
    }

    // tree ring (center)
    for (int y = terrain_height + trunk_height / 2; y <= terrain_height + 1 + trunk_height + center_ring_height;
         y++) {
    for (int i = localX - 3; i <= localX + 3; i++){
        for (int j = localZ - 3 ; j <= localZ + 3; j++) {
        if (i < 0 || j < 0 || i > 15 || j > 15) continue;
        if (i == localX && j == localZ && y < terrain_height + 1 + trunk_height) continue;
        float value = TerrainGenerator::random1(0.f, 1.f);
        if(abs(i - localX) < 2 && abs(j - localZ) < 2){
            chunk->setBlockAt(i, y, j, OAKLEAF);
        }
        float ratio = (y - (terrain_height + trunk_height / 2)) / (1 + trunk_height / 2 + center_ring_height);
        if(value < 0.3 * pow(1.0 - ratio, 2)){
            chunk->setBlockAt(i, y, j, OAKLEAF);
        }
        }
    }
    }

    // draw the top
    chunk->setBlockAt(localX,
                      terrain_height + 2 + trunk_height + center_ring_height,
                      localZ,
                      OAKLEAF);
}

bool Terrain::isChunkDirty(Chunk *ptr)
{
    return isChunkDirty(ptr->getKey());
}

bool Terrain::isChunkDirty(int64_t k)
{
    return dirtyChunksWaitingQueue.find(k) != dirtyChunksWaitingQueue.end()
           || dirtyChunksReadyBuild.find(k) != dirtyChunksReadyBuild.end();
}

void Terrain::setChunkDirty(Chunk *ptr)
{
    setChunkDirty(ptr->getKey());
}

void Terrain::setChunkDirty(int64_t k)
{
    if(isChunkIniting(k)){
        return;
    }

    if (m_chunkUnderVBOBuilding.find(k) == m_chunkUnderVBOBuilding.end()) {
        dirtyChunksReadyBuild.insert(k);
    } else {
        dirtyChunksWaitingQueue.insert(k);
    }
}

void Terrain::chunkVBOBuildStart(Chunk *ptr)
{
    chunkVBOBuildStart(ptr->getKey());
}

void Terrain::chunkVBOBuildStart(int64_t k)
{
    // Chunk should not be dirty when it is start rebinding draw datas
    if (!dirtyChunksReadyBuild.erase(k)) {
        throw std::invalid_argument("the chunk is not ready for build!");
    }
    if (isChunkDirty(k)) {
        throw std::invalid_argument("Chunk remains dirty after starting build!");
    }

    m_chunkUnderVBOBuilding.insert(k);
}

void Terrain::chunkVBOBuildComplete(Chunk *ptr)
{
    int64_t k = ptr->getKey();
    m_chunkUnderVBOBuilding.erase(k);
    if (isChunkDirty(ptr)) {
        dirtyChunksWaitingQueue.erase(k);
        dirtyChunksReadyBuild.insert(k);
    }
}

bool Terrain::isAllTaskComplete()
{
    return uninitChunks.empty() &&
           dirtyChunksReadyBuild.empty() &&
           dirtyChunksWaitingQueue.empty() &&
           m_chunkUnderVBOBuilding.empty();
}

void Terrain::allTerrainTaskComplete()
{
    /// Trigger Callback when all current task is done
    if (this->IsSigBlocked()) {
	return;
    }

    if(!m_onTerrainReady){
        return;
    }

    m_onTerrainReady();
}
