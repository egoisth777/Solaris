#ifndef TERRAINGENERATOR_H
#define TERRAINGENERATOR_H
#include "glm_includes.h"
#include <memory>
#include <vector>
enum NOISETYPE { PERLIN = 0, WORLEY = 1, NORMAL = 3 };

// define some constants
const int MAX_DEPTH_DELTA = 8;
const int MAX_DEPTH_STRAIGHT = 8;
const int TREE_DEPTH = 5;
const int RIVERMAX = 256;
const int RIVERMIN = 0;
const float MARCHSTEP_STRAIGHT = 28.f;
const float MARCHSTEP_DELTA = 28.f;
const int TREE_MAX = 128;
const int TREE_MIN = 0;

// some predeclaration
class LSystem;
class River;
typedef std::array<std::array<bool, RIVERMAX>, RIVERMAX> RiverMap;
typedef std::array<std::array<bool, TREE_MAX>, TREE_MAX> TreeMap;


/**
 * @brief The Helper class that stores all math helpers for random
 * Terrain Generation
 *
 * API Definition
 */
class TerrainGenerator
{
public:
    TerrainGenerator();
    ~TerrainGenerator();

  public: // some static math helpers for noise computation
    static inline uint8_t tablehash(int32_t i);
    static glm::vec2 inline vecpow(glm::vec2 v, int p)
    {
        glm::vec2 res{v};
        for (int i = 0; i < p - 1; i++) {
            res *= v;
        }
        return res;
    }

    // return a floating point number, faster than the
    // (int) std::floor(target)
    static int inline fastfloor(float target)
    {
        int i = static_cast<int>(target);
        return target < i ? i - 1 : i; // works for negative numbers
    }

    template<typename T>
    static T intervalRemapper(
        T num, T currLowerBound, T currUpperBound, T targetLowerBound, T targetUpperBound)
    {
        return (num - currLowerBound) / (currUpperBound - currLowerBound)
                   * (targetUpperBound - targetLowerBound)
               + targetLowerBound;
    }

  public:
    // well-formed basic noise functions
    static float noise1D(glm::vec2 p);
    static float noise2D(float x, float z);
    static int random1(int lowerBound, int upperBound);
    static float smoothNoise2D(float x, float z);
    static float linearInterp(float a, float b, float t);
    static float cosineInterp(float a, float b, float t);
    static float interpNoise2D(float x, float z);
    static float fbm2D(float x, float z, float persistence, NOISETYPE noiseType, int);


    // worley noise and its helper functions
    static const glm::vec2 voronoiCenter(glm::vec2 cornerPos);
    static float worleyNoise(float x, float z);

    // 2D Perlin Noise Helpers
    static float surflet(const glm::vec2&, const glm::vec2&, int);
    static glm::vec2 noiseVectorField(const glm::vec2& v, int primeSet);
    static float PerlinNoise2D(const glm::vec2& uv, int primeSet);

    // 3D Perlin Noise generation and its helpers
    static inline uint8_t permHash(int32_t i);
    static inline float grad(int32_t hash, float x, float y, float z);
    static inline float grad(int32_t hash, float x, float z);

    static float PerlinNoise3DBy2D(float x,
                                   float z,
                                   float y); // PerlinNoise Method by 2D PerlinNoise

    static float PerlinNoise3D(float x, float z, float y); // ground up PerlinNoise3DMethod

    // 2D Simplex Noise that saves more calculation overheads than Perlin Noise
    // this function is used for tree generation
    static float SimplexNoise2D(float x, float z);

    // in-hand mathematics tools for global usage
    // return a random angle in Radians in [0, PI/4]
    static float randAngle()
    {
	return M_PI / 8.0 * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }
    static float rand1D01() { return static_cast<float>(rand()) / static_cast<float>(RAND_MAX); }

  public:
    //static method for Terrain Generation
    void static sf_generatorInit();
    void static sf_setUpRivers();
    void static sf_generateRiverMap(LSystem* lsystem, RiverMap* riverMap);
    void static sf_generateGrandTreeMap(LSystem* lsystem, TreeMap* treeMap);
    void static sf_setUpTreeBluePrint();
    void static sf_drawTreeFixedType1(); // draw tree that has fixed pattern (non-procedural)s
  public:
    static int getHeight(int x, int z); // given a world coordinate x, z, return the corresponding height at the coordinate
    static int getGrassHeight(float x, float z); // given a world coordinate x, z, return the Grass terrain height for further blending
    static int getDesertHeight(
        float x,
        float z); // given a world coordinate x, z, return the Desert terrain height for further blending
    static bool isRiver(
        float x,
        float z); // given world coordinate x, z, return whether a certain chunk is river or not
    static bool isHole(
        int x,
        int z,
        int y); // given world coord x, z, y, return whether a given chunk is a hole or not (if hole, mark as empty)
    static int getMountainHeight(
	float x,
	float z); // given a world coordinate x, z, return the Mountain terrain height for further blending
    static void sd_printRiverMap(RiverMap* riverMap);

    static void sd_printTreeMap(TreeMap* treeMap);
  public:
    static std::unique_ptr<RiverMap> sp_riverMap;  // River with River Delta
    static std::unique_ptr<RiverMap> sp_riverMap2; // a straighter rive
    static std::unique_ptr<TreeMap> sp_treeMap; // A 2D L-system generated Tree (Grand Tree)
    static std::array<int, 4> offsets;
    friend class Terrain;
};

#endif // TERRAINGENERATOR_H
