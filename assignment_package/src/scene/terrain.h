#pragma once
#include "chunk.h"
#include "cube.h"
#include "glm_includes.h"
#include "shaderprograms/shaderprogram.h"
#include "smartpointerhelp.h"
#include "terraingenerator.h"
#include <Interface/ISignalEmitter.h>
#include <MultiThread/threadsafeset.h>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include "../settings.h"

#define MAX_HEIGHT 255

/**
 * E_AINPUT are different conditions given(x, y, z)
 * The current world pos of the a block
 * These input are used to form a Pattern that are parsed by the 
 * Automata to determine the Final Block Type of a specific block
 */
enum E_AINPUT {
  HEIGHT0_60 = 0,
  HEIGHT60_120 = 1,
  HEIGHT121_128 = 2,
  HEIGHT_128_160 = 3,
  HEIGHT160_250 = 4,
  ISTOP = 5,
  ISRIVER = 6,
};

//using namespace std;

// Helper functions to convert (x, z) to and from hash map key
int64_t toKey(int x, int z);
glm::ivec2 toCoords(int64_t k);

// The container class for all of the Chunks in the game.
// Ultimately, while Terrain will always store all Chunks,
// not all Chunks will be drawn at any given time as the world
// expands.
class Terrain : public ISignalEmitter {
public:
    int renderRadius    = TERRAIN_RENDER_RADIUS;
    int loadRadius      = TERRAIN_LOAD_RADIUS;
private:
    // Stores every Chunk according to the location of its lower-left corner
    // in world space.
    // We combine the X and Z coordinates of the Chunk's corner into one 64-bit int
    // so that we can use them as a key for the map, as objects like std::pairs or
    // glm::ivec2s are not hashable by default, so they cannot be used as keys.
    std::unordered_map<int64_t, uPtr<Chunk>> m_chunks;

    // We will designate every 64 x 64 area of the world's x-z plane
    // as one "terrain generation zone". Every time the player moves
    // near a portion of the world that has not yet been generated
    // (i.e. its lower-left coordinates are not in this set), a new
    // 4 x 4 collection of Chunks is created to represent that area
    // of the world.
    // The world that exists when the base code is run consists of exactly
    // one 64 x 64 area with its lower-left corner at (0, 0).
    // When milestone 1 has been implemented, the Player can move around the
    // world to add more "terrain generation zone" IDs to this set.
    // While only the 3 x 3 collection of terrain generation zones
    // surrounding the Player should be rendered, the Chunks
    // in the Terrain will never be deleted until the program is terminated.
    std::unordered_set<int64_t> m_generatedTerrain;

    // store chunks that are under VBO generating and binding
    std::unordered_set<int64_t> m_chunkUnderVBOBuilding;

    // store dirty chunks that needs to re-create VBO
    // they will not be re-create immediately until previou VBO building complete
    // Will only be accessed in the main thread
    std::unordered_set<int64_t> dirtyChunksWaitingQueue;

    // store dirty chunks that are ready to re-create build VBO
    // i.e. there is no recreating VBO or binding VBO happening to this chunk
    std::unordered_set<int64_t> dirtyChunksReadyBuild;

    // store dirty chunks that needs to initialize
    std::unordered_set<int64_t> uninitChunks;

    // Chunks that are under initing by any thread
    std::unordered_set<int64_t> m_initingChunk;

    glm::vec3 playerPos;

    // TODO: DELETE ALL REFERENCES TO m_geomCube AS YOU WILL NOT USE
    // IT IN YOUR FINAL PROGRAM!
    // The instance of a unit cube we can use to render any cube.
    // Presently, Terrain::draw renders one instance of this cube
    // for every non-EMPTY block within its Chunks. This is horribly
    // inefficient, and will cause your game to run very slowly until
    // milestone 1's Chunk VBO setup is completed.
    Cube m_geomCube;

    OpenGLContext* mp_context;

    std::unique_ptr<std::array<std::array<bool, RIVERMAX>, RIVERMAX>>
	mp_riverMap; // manage a resource rivermap

    /// Multi-thread variables
    /// Helps to save the result from children threads
    /// ONLY accessed by the main thread at every end of post-update
private:    
    ThreadSafeSet<int64_t>  m_ChunksByThreads;
    ThreadSafeSet<Chunk*>   m_ChunksVBOcreatedByThreads;


public:
    Terrain(OpenGLContext *context);
    ~Terrain();

    // Instantiates a new Chunk and stores it in
    // our chunk map at the given coordinates.
    // Returns a pointer to the created Chunk.
    Chunk* instantiateChunkAt(int x, int z);
    // Do these world-space coordinates lie within
    // a Chunk that exists?
    bool hasChunkAt(int x, int z) const;
    // Assuming a Chunk exists at these coords,
    // return a mutable reference to it
    uPtr<Chunk>& getChunkAt(int x, int z);
    // Assuming a Chunk exists at these coords,
    // return a const reference to it
    const uPtr<Chunk>& getChunkAt(int x, int z) const;
    // Given a world-space coordinate (which may have negative
    // values) return the block stored at that point in space.
    BlockType getBlockAt(int x, int y, int z) const;
    BlockType getBlockAt(glm::vec3 p) const;
    // Given a world-space coordinate (which may have negative
    // values) set the block at that point in space to the
    // given type.
    void setBlockAt(int x, int y, int z, BlockType t);

    // Draws every Chunk that falls within the bounding box
    // described by the min and max coords, using the provided
    // ShaderProgram
    void draw(int minX, int maxX, int minZ, int maxZ, ShaderProgram *shaderProgram);
    void draw(ShaderProgram *shaderProgram);
    void drawShadow(ShaderProgram *shaderProgram);
    void drawTransp(ShaderProgram *shaderProgram);
    // Initializes the Chunks that store the 64 x 256 x 64 block scene you
    // see when the base code is run.
    void CreateTestScene();
    void CreateVLTestScene(glm::vec3);
    void CreateLakeSword(glm::vec3);
    void DrawBall(glm::vec3 center, float radius, glm::vec3 scale, BlockType t);

    void updatePlayerPos(glm::vec3 pos);

    // load new chunks when the player approaches the border
    void loadNewChunkFromPlayer();

    // called at the end of a frame to update dirty chunks
    void postUpdate();

    // called at the begin of a frame to update dirty chunks
    void preUpdate();

    // set block while marking the chunk as dirty
    void playerPlaceBlock(int x, int y, int z, BlockType t);

    // break block while marking the chunk as dirty
    void playerBreakBlock(int x, int y, int z);


    /// CALLBACKs
    /// Notify gameMgr when all terrain task is done
    /// Generally used for generating a huge number of chunks
  private:
    std::function<void()> m_onTerrainReady = nullptr;

  public:
    void RegisterTerrainTaskCompleteAction(std::function<void()>);
    void UnregisterTerrainTaskCompleteAction();

  private:
    /// Collect the result of threads and do corresponding operations
    /// ONLY this function requires lock in the main thread
    void joinThreadResult();

    // Terrain generation helpers
    // STATIC
  protected: // related to Terrain Generation
    /// Create chunk for [ptr],
    /// and put its key into the set after completion,
    /// Safe to be handle by one thread
    static void threadCreateChunk(Chunk* ptr, ThreadSafeSet<int64_t>&);

    /// Create VBO data for the chunk pointed by [ptr],
    /// once complete, put it into the set.
    /// Safe to be handle by one thread
    static void threadChunkGenerateVBO(Chunk* ptr, ThreadSafeSet<Chunk*>&);
    static void sf_generateConditionPattern(std::vector<E_AINPUT>& pattern,
					    const int& x,
					    const int& y,
					    const int& z);
    static void sf_createTerrainChunk(
	Chunk* chunk); // generate a 16 x 16 size of the chunk of blocks wrp to the Terrain getHeight() function
    static BlockType getBlocktype(
	int x, int z, int curr_height, int terrain_height, bool ifTop, int offset);
    static void sf_PlantLTree(const int& localX,
			      const int& localZ,
			      const int& worldX,
			      const int& worldZ,
			      const int& terrain_height,
			      Chunk* chunk);
    static void sf_PlantNTree(const int& localX,
			      const int& localZ,
			      const int& worldX,
			      const int& WorldZ,
			      const int& terrain_height,
			      Chunk* chunk);
    static void sf_PlantBTree(const int& localX,
                              const int& localZ,
                              const int& worldX,
                              const int& WorldZ,
                              const int& terrain_height,
                              Chunk* chunk);
    static bool isTree(int worldX, int worldZ);
    static bool isBTree(int worldX, int worldZ);
    static bool sf_DrawRiver(); //@TODO
  private:
    void loadTerrainGenZone(int x, int z);
    void drawTerrainGenZone(ShaderProgram* shaderProgram, int x, int z, int mode);

    /// Check if a chunk is dirty
    /// A chunk is dirty if it is modified since the newest VBO generation
    bool isChunkDirty(Chunk*);
    bool isChunkDirty(int64_t);

    void setChunkDirty(Chunk*);
    void setChunkDirty(int64_t);

    bool isChunkIniting(int64_t);
    void chunkInitStart(int64_t);
    void chunkInitComplete(int64_t);

    void chunkVBOBuildStart(Chunk*);
    void chunkVBOBuildStart(int64_t);

    void chunkVBOBuildComplete(Chunk*);

    /// Check if there is no uninit / dirty / under-building chunk
    bool isAllTaskComplete();
    void allTerrainTaskComplete();
};
