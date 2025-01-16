#pragma once
#include "smartpointerhelp.h"
#include "glm_includes.h"
#include <array>
#include <unordered_map>
#include <cstddef>
#include <drawable.h>
#include "blocks.h"
#include <MultiThread/threadsafebase.h>

//using namespace std;
// One Chunk is a 16 x 256 x 16 section of the world,
// containing all the Minecraft blocks in that area.
// We divide the world into Chunks in order to make
// recomputing its VBO data faster by not having to
// render all the world at once, while also not having
// to render the world block by block.
// Chunk inherit from Drawable
class Chunk : public Drawable, public ThreadSafeBase
{
private:
    // All of the blocks contained within this Chunk
    std::array<BlockType, 65536> m_blocks;
    int minX, minZ;
    glm::vec3 cBase;
    // This Chunk's four neighbors to the north, south, east, and west
    // The third input to this map just lets us use a Direction as
    // a key for this map.
    // These allow us to properly determine
    std::unordered_map<Direction, Chunk*, EnumHash> m_neighbors;
    void insertFaceToVertBuf(std::vector<VertexData>&,
                         Direction, BlockType, glm::vec4) const;
    void insertPlantsToVertBuf(std::vector<VertexData>&,
                             BlockType, glm::vec4) const;
    BlockType getBlockNeighbour(Direction, int, int, int) const;

    // VBO helper
private:
    uPtr<std::vector<VertexData>> opqBuf     = nullptr;
    uPtr<std::vector<VertexData>> traBuf     = nullptr;
    uPtr<std::vector<GLuint>> idxBuf         = nullptr;
    uPtr<std::vector<GLuint>> traIdxBuf         = nullptr;

public:
    static glm::vec3 ChunkSize;

public:
    Chunk(OpenGLContext* context);
    Chunk(OpenGLContext*, int, int);
    glm::vec2 getOrigin() const;
    int64_t getKey() const;
    BlockType getBlockAt(unsigned int x, unsigned int y, unsigned int z) const;
    BlockType getBlockAt(int x, int y, int z) const;
    void setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t);
    void linkNeighbor(uPtr<Chunk>& neighbor, Direction dir);

    /// Create vertex and index buffer,
    /// set them to be pointed by "interleavedBuf" and "indexBuf"
    void createVBOdata() override;

    /// Bind the contents of VBO pointers, and clear them
    void bindVBOdata();

public: // inlined getters serving for retriving the information
    inline glm::ivec2 getWorldPos(){return glm::vec2{this->minX, this->minZ};}
};
