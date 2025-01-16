#ifndef BLOCKS_H
#define BLOCKS_H

#include <cstddef>
#include <glm_includes.h>
#include <array>
#include <unordered_map>
#include <unordered_set>

static const int CUB_IDX_COUNT = 36;
static const int CUB_VERT_COUNT = 24;

// Lets us use any enum class as the key of a
// std::unordered_map
struct EnumHash {
    template <typename T>
    size_t operator()(T t) const {
        return static_cast<size_t>(t);
    }
};

// deprecated. Not suitable for further extension
#define TP 0x80
#define NOT_COLLIDE 0x40

// C++ 11 allows us to define the size of an enum. This lets us use only one byte
// of memory to store our different block types. By default, the size of a C++ enum
// is that of an int (so, usually four bytes). This *does* limit us to only 256 different
// block types, but in the scope of this project we'll never get anywhere near that many.
enum BlockType : unsigned char
{
    EMPTY    = 0,
    STONE    = 1,
    GRASS    = 2,
    DIRT     = 3,
    WATER    = 4,
    BEDROCK  = 5,
    SAND     = 6,
    SNOW     = 7,
    LAVA     = 8,
    OAKLOG   = 9,
    OAKLEAF  = 10,
    BIRCHLOG = 11,
    WILDGRASS= 12,
    POPPY    = 13,
    DANDELION= 14,
    GOLDBLOCK= 15,
    IRONBLOCK= 16,
    OBSIDIAN = 17,
    COBBLE   = 18,
    MOSSYCOBBLE = 19,

};

static const std::unordered_set<BlockType, EnumHash> transpMap
{
    EMPTY, WATER
};

static const std::unordered_set<BlockType, EnumHash> noCollideMap
{
    EMPTY, WATER, LAVA, WILDGRASS, POPPY, DANDELION
};

static const std::unordered_set<BlockType, EnumHash> noCullMap
{
    EMPTY, WATER, LAVA, WILDGRASS, POPPY, DANDELION, OAKLEAF
};

static const std::unordered_set<BlockType, EnumHash> plantsMap
{
    WILDGRASS, POPPY, DANDELION
};

inline bool isTransP(BlockType t)
{
    return transpMap.find(t) != transpMap.end();
}

inline bool isCollider(BlockType t)
{
    return noCollideMap.find(t) == noCollideMap.end();
}

inline bool isCulled(BlockType t)
{
    return noCullMap.find(t) == noCullMap.end();
}

inline bool isPlants(BlockType t)
{
    return plantsMap.find(t) != plantsMap.end();
}


// The six cardinal directions in 3D space
enum Direction : unsigned char
{
    ZPOS = 0,
    XPOS = 1,
    XNEG = 2,
    ZNEG = 3,
    YPOS = 4,
    YNEG = 5
};

static const std::array<glm::vec3, 6> DirVec
{
    glm::vec3(0, 0, 1),
    glm::vec3(1, 0, 0),
    glm::vec3(-1, 0, 0),
    glm::vec3(0, 0, -1),
    glm::vec3(0, 1, 0),
    glm::vec3(0, -1, 0)
};

static const std::array<glm::vec4, 24> CubeVert
{
    //Front face
    glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
    glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
    glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),

    //Right face
    glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
    glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),

    //Left face
    glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
    glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),

    //Back face
    glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),

    //Top face
    glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
    glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
    glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
    glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),

    //Bottom face
    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)

};

static const std::array<glm::vec4, 24> CubeNor
{
    //Front
    glm::vec4(0,0,1,0),
    glm::vec4(0,0,1,0),
    glm::vec4(0,0,1,0),
    glm::vec4(0,0,1,0),
    //Right
    glm::vec4(1,0,0,0),
    glm::vec4(1,0,0,0),
    glm::vec4(1,0,0,0),
    glm::vec4(1,0,0,0),
    //Left
    glm::vec4(-1,0,0,0),
    glm::vec4(-1,0,0,0),
    glm::vec4(-1,0,0,0),
    glm::vec4(-1,0,0,0),
    //Back
    glm::vec4(0,0,-1,0),
    glm::vec4(0,0,-1,0),
    glm::vec4(0,0,-1,0),
    glm::vec4(0,0,-1,0),
    //Top
    glm::vec4(0,1,0,0),
    glm::vec4(0,1,0,0),
    glm::vec4(0,1,0,0),
    glm::vec4(0,1,0,0),
    //Bottom
    glm::vec4(0,-1,0,0),
    glm::vec4(0,-1,0,0),
    glm::vec4(0,-1,0,0),
    glm::vec4(0,-1,0,0)
};

static const std::array<int, 36> CubeIdx
{
    0 , 1 , 2 , 0 , 2 , 3 ,
    4 , 5 , 6 , 4 , 6 , 7 ,
    8 , 9 , 10, 8 , 10, 11,
    12, 13, 14, 12, 14, 15,
    16, 17, 18, 16, 18, 19,
    20, 21, 22, 20, 22, 23
};

static const std::array<glm::vec4, 8> PlantsVert
{
    glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),

    glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
    glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
};

static const std::array<glm::vec4, 8> PlantsNor
{
    glm::vec4(1.0f, 0.0f, 1.0f, 0.f),
    glm::vec4(1.0f, 0.0f, 1.0f, 0.f),
    glm::vec4(1.0f, 0.0f, 1.0f, 0.f),
    glm::vec4(1.0f, 0.0f, 1.0f, 0.f),

    glm::vec4(1.0f, 0.0f, -1.0f, 0.f),
    glm::vec4(1.0f, 0.0f, -1.0f, 0.f),
    glm::vec4(1.0f, 0.0f, -1.0f, 0.f),
    glm::vec4(1.0f, 0.0f, -1.0f, 0.f)
};

static const std::unordered_map<BlockType, glm::vec4, EnumHash> colMap
{
    {EMPTY, glm::vec4(0)},
    {STONE, glm::vec4(0.5f, 0.5f, 0.5f, 1.f)},
    {GRASS, glm::vec4(95.f, 159.f, 53.f, 255.f) / 255.f},
    {DIRT,  glm::vec4(121.f, 85.f, 58.f, 255.f) / 255.f},
    {WATER, glm::vec4(0.f, 0.f, 0.75f, 0.5f)},

};

// six number refers to the texture coord of six faces of a cube
// orders are: front->right->left->back->top->bottom
static const std::unordered_map<BlockType, std::array<int, 6>, EnumHash> texMap
{
    {EMPTY,     {0, 0, 0, 0, 0, 0}},
    {STONE,     {1, 1, 1, 1, 1, 1}},
    {GRASS,     {3, 3, 3, 3, 40, 2}},
    {DIRT,      {2, 2, 2, 2, 2, 2}},
    {WATER,     {205, 205, 205, 205, 205, 205}},
    {BEDROCK,   {17, 17, 17, 17, 17, 17}},
    {SAND,      {18, 18, 18, 18, 18, 18}},
    {SNOW,      {66, 66, 66, 66, 66, 66}},
    {LAVA,      {237, 237, 237, 237, 237, 237}},
    {OAKLOG,    {20, 20, 20, 20, 21, 21}},
    {OAKLEAF,   {53, 53, 53, 53, 53, 53}},
    {BIRCHLOG,  {117, 117, 117, 117, 117, 117}},
    {WILDGRASS, {39, 39, 39, 39, 39, 39}},
    {POPPY,     {12, 12, 12, 12, 12, 12}},
    {DANDELION, {13, 13, 13, 13, 13, 13}},
    {GOLDBLOCK, {23, 23, 23, 23, 23, 23}},
    {IRONBLOCK, {22, 22, 22, 22, 22, 22}},
    {OBSIDIAN,  {37, 37, 37, 37, 37, 37}},
    {COBBLE,    {16, 16, 16, 16, 16, 16}},
    {MOSSYCOBBLE, {36, 36, 36, 36, 36, 36}}
};

static const std::array<glm::vec2, 4> uvOffset
{
    glm::vec2{1.f/16.f, 0.f},
    glm::vec2{1.f/16.f, 1.f/16.f},
    glm::vec2{0.f, 1.f/16.f},
    glm::vec2{0.f, 0.f},

};

// returns the tex coord of the left-bottom corner
inline glm::vec2 getTexCoord(int id)
{
    return {(float)(id % 16) / 16.f, (float)(id / 16) / 16.f};
}

#endif // BLOCKS_H
