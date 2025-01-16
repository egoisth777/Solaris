#include "chunk.h"
#include <iostream>

glm::vec3 Chunk::ChunkSize = glm::vec3{16, 256, 16};

Chunk::Chunk(OpenGLContext* context, int x, int z) :
    Drawable(context), ThreadSafeBase(), m_blocks(), minX(x), minZ(z), cBase(glm::vec3(minX, 0, minZ)),
    m_neighbors{{XPOS, nullptr}, {XNEG, nullptr}, {ZPOS, nullptr}, {ZNEG, nullptr}}
{
    std::fill_n(m_blocks.begin(), 65536, EMPTY);
    opqBuf      = mkU<std::vector<VertexData>>();
    traBuf      = mkU<std::vector<VertexData>>();
    idxBuf      = mkU<std::vector<GLuint>>();
    traIdxBuf   = mkU<std::vector<GLuint>>();
}

glm::vec2 Chunk::getOrigin() const
{
    return {minX, minZ};
}

int64_t Chunk::getKey() const
{
    int64_t xz = 0xffffffffffffffff;
    int64_t x64 = minX;
    int64_t z64 = minZ;

    // Set all lower 32 bits to 1 so we can & with Z later
    xz = (xz & (x64 << 32)) | 0x00000000ffffffff;

    // Set all upper 32 bits to 1 so we can & with XZ
    z64 = z64 | 0xffffffff00000000;

    // Combine
    xz = xz & z64;
    return xz;
}

// Does bounds checking with at()
BlockType Chunk::getBlockAt(unsigned int x, unsigned int y, unsigned int z) const {
    return m_blocks.at(x + 16 * y + 16 * 256 * z);
}

// Exists to get rid of compiler warnings about int -> unsigned int implicit conversion
BlockType Chunk::getBlockAt(int x, int y, int z) const {
    return getBlockAt(static_cast<unsigned int>(x), static_cast<unsigned int>(y), static_cast<unsigned int>(z));
}

// Does bounds checking with at()
void Chunk::setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t) {
    try{
        m_blocks.at(x + 16 * y + 16 * 256 * z) = t;
    }
    catch (std::out_of_range& e){
        std::cerr << e.what() << std::endl;
    }
}

// assume we only have neighbours at x and z axis
BlockType Chunk::getBlockNeighbour(Direction dir, int x, int y, int z) const
{
    // return empty if no neighbour
    if (m_neighbors.at(dir) == nullptr) return EMPTY;

    Chunk* neighbour = m_neighbors.at(dir);
    switch (dir) {
    case ZPOS:
        return neighbour->getBlockAt(x, y, z - 16);
        break;
    case XPOS:
        return neighbour->getBlockAt(x - 16, y, z);
        break;
    case XNEG:
        return neighbour->getBlockAt(x + 16, y, z);
        break;
    case ZNEG:
        return neighbour->getBlockAt(x, y, z + 16);
        break;
    default:
        break;
    }
    return EMPTY;
}


const static std::unordered_map<Direction, Direction, EnumHash> oppositeDirection {
    {XPOS, XNEG},
    {XNEG, XPOS},
    {YPOS, YNEG},
    {YNEG, YPOS},
    {ZPOS, ZNEG},
    {ZNEG, ZPOS}
};

void Chunk::linkNeighbor(uPtr<Chunk> &neighbor, Direction dir) {
    if(neighbor != nullptr) {
        this->m_neighbors[dir] = neighbor.get();
        neighbor->m_neighbors[oppositeDirection.at(dir)] = this;
    }
}

void Chunk::insertFaceToVertBuf(std::vector<VertexData>& buf,
                     Direction dir, BlockType t, glm::vec4 p) const
{
    int bufIdx = dir * 4;
    glm::vec2 texBase = getTexCoord(texMap.at(t)[static_cast<int>(dir)]);
    for (int i = 0; i < 4; ++i)
    {
        buf.emplace_back(
            p + CubeVert[bufIdx + i],
            CubeNor[bufIdx], texBase + uvOffset[i],
            (int)t);
    }
}

void Chunk::insertPlantsToVertBuf(std::vector<VertexData>& buf,
                           BlockType t, glm::vec4 p) const
{
    glm::vec2 texBase = getTexCoord(texMap.at(t)[0]);
    int bufIdx = 0;
    int type = t & 0x80;
    for (int i = 0; i < 4; ++i)
    {
        if (PlantsVert[bufIdx + i].y > 0) type  = t | 0x80;
        else type = t;

        buf.emplace_back(
            p + PlantsVert[bufIdx + i],
            PlantsNor[bufIdx], texBase + uvOffset[i],
            (int)type);
    }
    for (int i = 3; i >= 0; --i)
    {
        if (PlantsVert[bufIdx + i].y > 0) type  = t | 0x80;
        else type = t;

        buf.emplace_back(
            p + PlantsVert[bufIdx + i],
            -PlantsNor[bufIdx], texBase + uvOffset[i],
            (int)type);
    }
    bufIdx = 4;
    for (int i = 0; i < 4; ++i)
    {
        if (PlantsVert[bufIdx + i].y > 0) type  = t | 0x80;
        else type = t;

        buf.emplace_back(
            p + PlantsVert[bufIdx + i],
            PlantsNor[bufIdx], texBase + uvOffset[i],
            (int)type);
    }
    for (int i = 3; i >= 0; --i)
    {
        if (PlantsVert[bufIdx + i].y > 0) type  = t | 0x80;
        else type = t;

        buf.emplace_back(
            p + PlantsVert[bufIdx + i],
            -PlantsNor[bufIdx], texBase + uvOffset[i],
            (int)type);
    }
}

void Chunk::createVBOdata() {
    /// In general, there is 1 thread to modify 1 chunk
    /// But I add lock here to prevent unintended operation

    ready = false;
    std::lock_guard<std::mutex> lock(m_lock);
    opqBuf->clear();
    traBuf->clear();
    idxBuf->clear();
    traIdxBuf->clear();

    // remember to follow the zyx order
    // to ensure continuous array access
    for (int z = 0; z < 16;  ++z) {
        for (int y = 0; y < 256; ++y) {
            for (int x = 0; x < 16;  ++x) {
                BlockType t = getBlockAt(x, y, z);
                if (t == EMPTY) continue;

                glm::vec4 blockPos = glm::vec4(glm::vec3(x,y,z) + cBase, 0.f);
                BlockType nbr;

                if (isPlants(t))
                {
                    insertPlantsToVertBuf(*opqBuf, t, blockPos);
                    continue;
                }

                // check front face
                nbr = (z == 15) ? getBlockNeighbour(ZPOS, x, y, z + 1) : getBlockAt(x, y, z + 1);
                if (!isCulled(nbr))
                {
                    // if the block is solid
                    if (isCulled(t)) insertFaceToVertBuf(*opqBuf, ZPOS, t, blockPos);
                    // if block is not the same with neighbour
                    else if (t != nbr) insertFaceToVertBuf((isTransP(t) ? *traBuf : *opqBuf), ZPOS, t, blockPos);
                    // if it is water and its neighbour is not water
                    //else if (nbr != WATER) insertFaceToVertBuf(*traBuf, ZPOS, t, blockPos);
                }

                // check right face
                nbr = (x == 15) ? getBlockNeighbour(XPOS, x + 1, y, z) : getBlockAt(x + 1, y, z);
                if (!isCulled(nbr))
                {
                    if (isCulled(t)) insertFaceToVertBuf(*opqBuf, XPOS, t, blockPos);
                    else if (t != nbr) insertFaceToVertBuf((isTransP(t) ? *traBuf : *opqBuf), XPOS, t, blockPos);
                    //else if (nbr != WATER) insertFaceToVertBuf(*traBuf, XPOS, t, blockPos);
                }

                // check left face
                nbr = (x == 0) ? getBlockNeighbour(XNEG, x - 1, y, z) : getBlockAt(x - 1, y, z);
                if (!isCulled(nbr))
                {
                    if (isCulled(t)) insertFaceToVertBuf(*opqBuf, XNEG, t, blockPos);
                    else if (t != nbr) insertFaceToVertBuf((isTransP(t) ? *traBuf : *opqBuf), XNEG, t, blockPos);
                    //else if (nbr != WATER) insertFaceToVertBuf(*traBuf, XNEG, t, blockPos);
                }

                // check back face
                nbr = (z == 0) ? getBlockNeighbour(ZNEG, x, y, z - 1) : getBlockAt(x, y, z - 1);
                if (!isCulled(nbr))
                {
                    if (isCulled(t)) insertFaceToVertBuf(*opqBuf, ZNEG, t, blockPos);
                    else if (t != nbr) insertFaceToVertBuf((isTransP(t) ? *traBuf : *opqBuf), ZNEG, t, blockPos);
                    //else if (nbr != WATER) insertFaceToVertBuf(*traBuf, ZNEG, t, blockPos);
                }

                // check top face
                nbr = (y == 255) ? EMPTY : getBlockAt(x, y + 1, z);
                if (!isCulled(nbr))
                {
                    if (isCulled(t)) insertFaceToVertBuf(*opqBuf, YPOS, t, blockPos);
                    else if (t != nbr) insertFaceToVertBuf((isTransP(t) ? *traBuf : *opqBuf), YPOS, t, blockPos);
                    //else if (nbr != WATER) insertFaceToVertBuf(*traBuf, YPOS, t, blockPos);
                }

                // check bottom face
                nbr = (y == 0) ? EMPTY : getBlockAt(x, y - 1, z);
                if (!isCulled(nbr))
                {
                    if (isCulled(t)) insertFaceToVertBuf(*opqBuf, YNEG, t, blockPos);
                    else if (t != nbr) insertFaceToVertBuf((isTransP(t) ? *traBuf : *opqBuf), YNEG, t, blockPos);
                    //else if (nbr != WATER) insertFaceToVertBuf(*traBuf, YNEG, t, blockPos);
                }
    } } }

    int faceCnt = opqBuf->size() / 4;
    for (int i = 0; i < faceCnt; ++i)
    {
        idxBuf->push_back(i*4);
        idxBuf->push_back(i*4+1);
        idxBuf->push_back(i*4+2);
        idxBuf->push_back(i*4);
        idxBuf->push_back(i*4+2);
        idxBuf->push_back(i*4+3);
    }
    m_count = faceCnt * 6;

    faceCnt = traBuf->size() / 4;
    for (int i = 0; i < faceCnt; ++i)
    {
        traIdxBuf->push_back(i*4);
        traIdxBuf->push_back(i*4+1);
        traIdxBuf->push_back(i*4+2);
        traIdxBuf->push_back(i*4);
        traIdxBuf->push_back(i*4+2);
        traIdxBuf->push_back(i*4+3);
    }
    m_traCnt = faceCnt * 6;
}

void Chunk::bindVBOdata()
{
    // create cube VBO
    if(!m_idxGenerated) {
        generateIdx();
    }

    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_count * sizeof(GLuint), idxBuf->data(), GL_STATIC_DRAW);
    mp_context->printGLErrorLog();

    if(!m_vertGenerated) {
        generateVert();
    }

    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufVert);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, opqBuf->size() * sizeof(VertexData), opqBuf->data(), GL_STATIC_DRAW);
    mp_context->printGLErrorLog();

    if(!m_traIdxGenerated) {
        generateTraIdx();
    }

    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufTraIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_traCnt * sizeof(GLuint), traIdxBuf->data(), GL_STATIC_DRAW);
    mp_context->printGLErrorLog();

    if(!m_transpGenerated) {
        generateTransp();
    }

    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufTransp);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, traBuf->size() * sizeof(VertexData), traBuf->data(), GL_STATIC_DRAW);
    mp_context->printGLErrorLog();

    ready = true;
}
