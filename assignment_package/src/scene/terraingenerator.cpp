/**
 * simply a collection of static methods 
 * including noise generation, interpolation, etc
 * that facilitates the terrain generation
 * Most frequently, the output of these functions are heights of the block, 
 * or whether a block should be placed at a given world 3D space
*/

#include "terraingenerator.h"
#include "lsystem.h"
#include "river.h"
#include <chrono>
#include <cmath>
#include <iostream>
#include <math.h>
#include <qmath.h>
#include <random>
#include "tree.h"

TerrainGenerator::TerrainGenerator() {}
TerrainGenerator::~TerrainGenerator() {}
std::unique_ptr<RiverMap> TerrainGenerator::sp_riverMap = nullptr;
std::unique_ptr<RiverMap> TerrainGenerator::sp_riverMap2 = nullptr;
std::unique_ptr<TreeMap> TerrainGenerator::sp_treeMap = nullptr;
std::array<int, 4> TerrainGenerator::offsets = {12, 10, 10, 8};

// an uint8_t perm table for Perlin Noise generation
static const uint8_t PERM[256]
    = {151, 160, 137, 91,  90,  15,  131, 13,  201, 95,  96,  53,  194, 233, 7,   225, 140, 36,
       103, 30,  69,  142, 8,   99,  37,  240, 21,  10,  23,  190, 6,   148, 247, 120, 234, 75,
       0,   26,  197, 62,  94,  252, 219, 203, 117, 35,  11,  32,  57,  177, 33,  88,  237, 149,
       56,  87,  174, 20,  125, 136, 171, 168, 68,  175, 74,  165, 71,  134, 139, 48,  27,  166,
       77,  146, 158, 231, 83,  111, 229, 122, 60,  211, 133, 230, 220, 105, 92,  41,  55,  46,
       245, 40,  244, 102, 143, 54,  65,  25,  63,  161, 1,   216, 80,  73,  209, 76,  132, 187,
       208, 89,  18,  169, 200, 196, 135, 130, 116, 188, 159, 86,  164, 100, 109, 198, 173, 186,
       3,   64,  52,  217, 226, 250, 124, 123, 5,   202, 38,  147, 118, 126, 255, 82,  85,  212,
       207, 206, 59,  227, 47,  16,  58,  17,  182, 189, 28,  42,  223, 183, 170, 213, 119, 248,
       152, 2,   44,  154, 163, 70,  221, 153, 101, 155, 167, 43,  172, 9,   129, 22,  39,  253,
       19,  98,  108, 110, 79,  113, 224, 232, 178, 185, 112, 104, 218, 246, 97,  228, 251, 34,
       242, 193, 238, 210, 144, 12,  191, 179, 162, 241, 81,  51,  145, 235, 249, 14,  239, 107,
       49,  192, 214, 31,  181, 199, 106, 157, 184, 84,  204, 176, 115, 121, 50,  45,  127, 4,
       150, 254, 138, 236, 205, 93,  222, 114, 67,  29,  24,  72,  243, 141, 128, 195, 78,  66,
       215, 61,  156, 180};

// well-formed basic noise functions
/**
 * @brief return the normal 1D noise
 * @param p
 * @return
 */
float TerrainGenerator:: noise1D( glm::vec2 p ) {
    return glm::fract(sin(glm::dot(p, glm::vec2(127.1, 311.7))) * 43758.5453);
}

/**
 * @brief return a smoother version of the noise2D
 * @param x some confined coord, could be in any coord space
 * @param z some confined coord, could be in any coord space
 * @return
 */
float TerrainGenerator :: noise2D(float x, float z){
    float s = sin(glm::dot(glm::vec2(x, z), glm::vec2(127.1, 311.7))) * 43758.5453;
    return modf(s, nullptr);
}

int TerrainGenerator::random1(int lowerBound, int upperBound)
{
    std::random_device rd;
    std::mt19937 eng(rd());

    // Define the range
    int lower_bound = lowerBound; // Define your lower bound
    int upper_bound = upperBound; // Define your upper bound

    std::uniform_int_distribution<> distr(lower_bound, upper_bound);

    // Generate and print a random integer
    return distr(eng);
}
/**
 * @brief return the smoothed version of the noise2D
 * @param x some confined coord, could be in any coord space
 * @param z some confined coord, could be in any coord space
 * @return
 */
float TerrainGenerator :: smoothNoise2D(float x, float z){
    float cornerVal = (noise2D(x - 1, z - 1) +
                       noise2D(x + 1, z - 1) +
                       noise2D(x - 1, z + 1) +
                       noise2D(x + 1, z + 1)) / 16;
    float sidesVal = (noise2D(x - 1, z) +
                      noise2D(x + 1, z) +
                      noise2D(x, z - 1) +
                      noise2D(x, z + 1)) / 8;
    float centerVal = noise2D(x, z) / 4;

    return cornerVal + sidesVal + centerVal;
}

/**
 * @brief linearInterp
 * @param a
 * @param b
 * @param t
 * @return
 */
float TerrainGenerator::linearInterp(float a, float b, float t){
    return a*(1 - t) +  b * t;
}

/**
 * @brief cosineInterp
 * @param a
 * @param b
 * @param t
 * @return
 */
float TerrainGenerator::cosineInterp(float a, float b, float t){
    t = (1 - cos(t * M_PI)) * 0.5;
    return linearInterp(a, b, t);
}


/**
 * @brief TerrainGenerator::interpNoise2D
 * @param x
 * @param z
 * @return
 */
float TerrainGenerator::interpNoise2D(float x, float z){
    float intX, fractX;
    fractX = std::modf(x, &intX);

    float intZ, fractZ;
    fractZ = std::modf(z, &intZ);

    float v1 = smoothNoise2D(intX, intZ);
    float v2 = smoothNoise2D(intX + 1, intZ);
    float v3 = smoothNoise2D(intX, intZ + 1);
    float v4 = smoothNoise2D(intX + 1, intZ + 1);

    float i1 = cosineInterp(v1, v2, fractX);
    float i2 = cosineInterp(v3, v4, fractX);

    return cosineInterp(i1, i2, fractZ);
}

/**
 * @brief TerrainGenerator::fbm2D
 * @param x
 * @param z
 * @param persistence
 * @param noiseFn
 * @param primeSet
 * @return
 */
float TerrainGenerator::fbm2D(float x, float z, float persistence, NOISETYPE noiseType, int primeSet){
    float total = 0;
    int octaves = 8;

    for (int i = 0; i < octaves; i++) {
        float frequency = pow(2, i);
        float amplitude = pow(persistence, i);

        if (noiseType == PERLIN) { // passed in function = PERLIN,
            total += TerrainGenerator::PerlinNoise2D(glm::vec2(x * frequency, z * frequency), primeSet) * amplitude;
        }
        if (noiseType == NORMAL) { // passed in function == NORMAL, use normal 2D noise generator
            total += interpNoise2D(x * frequency, z * frequency) * amplitude;
        }
    }
    return total;
}


// Perlin Noise Helpers and Perlin Noise Calculator
/**
 * @brief noiseVectorField
 * @param v
 * @param primeSet
 * @return
 */
glm::vec2 TerrainGenerator::noiseVectorField(const glm::vec2& v, int primeSet){
    glm::vec2 vec = v;
    vec += 0.1;

    float xMult;
    float yMult;
    glm::mat2 primes;

    if (primeSet == 1) {
        primes = glm::mat2{{126.1, 311.7}, {420.2, 1337.1}};
        xMult = 43758.5453;
        yMult = 789221.5453;
    }

    if (primeSet == 2) {
        primes = glm::mat2{{593.32, 931.85}, {719.31, 1029.44}};
        xMult = 354234.5048;
        yMult = 250986.2095;
    }

    if (primeSet == 3) {
        primes = glm::mat2{{958.11, 347.77}, {139.44, 9559.43}};
        xMult = 485048.09604;
        yMult = 9450.234234;
    }

    glm::vec2 noise = glm::sin(vec * primes);
    noise.x *= xMult;
    noise.y *= yMult;

    return glm::normalize(glm::abs(glm::fract(noise)));
}

float TerrainGenerator:: surflet(const glm::vec2& p, const glm::vec2& gridPoint, int primeSet){
    glm::vec2 t2 = glm::abs(p - gridPoint);
    glm::vec2 t = glm::vec2(1.f) - 6.f * vecpow(t2, 5) + 15.f * vecpow(t2, 4)
                  - 10.f * vecpow(t2, 3);

    glm::vec2 gradient = noiseVectorField(gridPoint, primeSet) * 2.f - glm::vec2(1,1);
    glm::vec2 diff = p - gridPoint;

    float height = glm::dot(diff, gradient);

    return height * t.x * t.y;
}

float TerrainGenerator::PerlinNoise2D(const glm::vec2& uv, int primeSet)
{
    float surfletSum = 0.f;

    for (int dx = 0; dx <= 1; ++dx) {
        for (int dy = 0; dy <= 1; ++dy) {
            surfletSum += surflet(uv, glm::floor(uv) + glm::vec2(dx, dy), primeSet);
        }
    }

    return surfletSum;
}

/**
 * Note: Deprecated, cost is too heavy
 * @brief PerlinNoise2D generate 2D PerlinNoise
 * @param uv the uv coordinate
 * @param primeSet the Primeset help to generate 2D perlin
 * @return a random float ranging from [-1, 1]
 * Too much overhead
 */
float TerrainGenerator::PerlinNoise3DBy2D(float x, float z, float y)
{
    float ab = PerlinNoise2D(glm::vec2{x, y}, 1);
    float bc = PerlinNoise2D(glm::vec2{y, z}, 1);
    float ac = PerlinNoise2D(glm::vec2{x, z}, 1);
    float ba = PerlinNoise2D(glm::vec2{y, x}, 1);
    float cb = PerlinNoise2D(glm::vec2{z, y}, 1);
    float ca = PerlinNoise2D(glm::vec2{z, x}, 1);

    float abc = ab + bc + ac + ba + cb + ca;

    return abc / 6.f;
}

/**
 * @brief Hash indexing into the permutation table
 * @param i integer value to hash
 * @return 8-bits hashed value
 */
inline uint8_t TerrainGenerator::permHash(int32_t i)
{
    return PERM[static_cast<uint8_t>(i)];
}

/**
 * @brief gradient
 * @param hash
 * @param x
 * @return
 */
float TerrainGenerator::grad(int32_t hash, float x, float y, float z)
{
    int h = hash & 15;       // Convert low 4 bits of hash code into 12 simple
    float u = h < 8 ? x : y; // gradient directions, and compute dot product.
    float v = h < 4 ? y : h == 12 || h == 14 ? x : z; // Fix repeats at h = 12 to 15
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

/**
 * @brief Overloaded 2D gradient helper
 * @param hash
 * @param x
 * @param z
 * @return 
 */
float TerrainGenerator::grad(int32_t hash, float x, float z)
{
    const int32_t h = hash & 0x3F; // Convert low 3 bits of hash code
    const float u = h < 4 ? x : z; // into 8 simple gradient directions,
    const float v = h < 4 ? z : x;
    return ((h & 1) ? -u : u)
	   + ((h & 2) ? -2.0f * v : 2.0f * v); // and compute the dot product with (x,y).
}

/**
 * @brief TerrainGenerator::PerlinNoise3D A full implementation of
 * @param x
 * @param z
 * @param y
 * @return
 */
float TerrainGenerator::PerlinNoise3D(float x, float y, float z)
{
    float n0, n1, n2, n3; // Noise contributions from the four corners

    // Skewing/Unskewing factors for 3D
    static const float F3 = 1.0f / 3.0f;
    static const float G3 = 1.0f / 6.0f;

    // Skew the input space to determine which simplex cell we're in
    float s = (x + y + z) * F3; // Very nice and simple skew factor for 3D
    int i = fastfloor(x + s);
    int j = fastfloor(y + s);
    int k = fastfloor(z + s);
    float t = (i + j + k) * G3;
    float X0 = i - t; // Unskew the cell origin back to (x,y,z) space
    float Y0 = j - t;
    float Z0 = k - t;
    float x0 = x - X0; // The x,y,z distances from the cell origin
    float y0 = y - Y0;
    float z0 = z - Z0;

    // For the 3D case, the simplex shape is a slightly irregular tetrahedron.
    // Determine which simplex we are in.
    int i1, j1, k1; // Offsets for second corner of simplex in (i,j,k) coords
    int i2, j2, k2; // Offsets for third corner of simplex in (i,j,k) coords
    if (x0 >= y0) {
        if (y0 >= z0) {
            i1 = 1;
            j1 = 0;
            k1 = 0;
            i2 = 1;
            j2 = 1;
            k2 = 0; // X Y Z order
        } else if (x0 >= z0) {
            i1 = 1;
            j1 = 0;
            k1 = 0;
            i2 = 1;
            j2 = 0;
            k2 = 1; // X Z Y order
        } else {
            i1 = 0;
            j1 = 0;
            k1 = 1;
            i2 = 1;
            j2 = 0;
            k2 = 1; // Z X Y order
        }
    } else { // x0<y0
        if (y0 < z0) {
            i1 = 0;
            j1 = 0;
            k1 = 1;
            i2 = 0;
            j2 = 1;
            k2 = 1; // Z Y X order
        } else if (x0 < z0) {
            i1 = 0;
            j1 = 1;
            k1 = 0;
            i2 = 0;
            j2 = 1;
            k2 = 1; // Y Z X order
        } else {
            i1 = 0;
            j1 = 1;
            k1 = 0;
            i2 = 1;
            j2 = 1;
            k2 = 0; // Y X Z order
        }
    }

    // A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
    // a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
    // a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
    // c = 1/6.
    float x1 = x0 - i1 + G3; // Offsets for second corner in (x,y,z) coords
    float y1 = y0 - j1 + G3;
    float z1 = z0 - k1 + G3;
    float x2 = x0 - i2 + 2.0f * G3; // Offsets for third corner in (x,y,z) coords
    float y2 = y0 - j2 + 2.0f * G3;
    float z2 = z0 - k2 + 2.0f * G3;
    float x3 = x0 - 1.0f + 3.0f * G3; // Offsets for last corner in (x,y,z) coords
    float y3 = y0 - 1.0f + 3.0f * G3;
    float z3 = z0 - 1.0f + 3.0f * G3;

    // Work out the hashed gradient indices of the four simplex corners
    int gi0 = permHash(i + permHash(j + permHash(k)));
    int gi1 = permHash(i + i1 + permHash(j + j1 + permHash(k + k1)));
    int gi2 = permHash(i + i2 + permHash(j + j2 + permHash(k + k2)));
    int gi3 = permHash(i + 1 + permHash(j + 1 + permHash(k + 1)));

    // Calculate the contribution from the four corners
    float t0 = 0.6f - x0 * x0 - y0 * y0 - z0 * z0;
    if (t0 < 0) {
        n0 = 0.0;
    } else {
        t0 *= t0;
        n0 = t0 * t0 * grad(gi0, x0, y0, z0);
    }
    float t1 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1;
    if (t1 < 0) {
        n1 = 0.0;
    } else {
        t1 *= t1;
        n1 = t1 * t1 * grad(gi1, x1, y1, z1);
    }
    float t2 = 0.6f - x2 * x2 - y2 * y2 - z2 * z2;
    if (t2 < 0) {
        n2 = 0.0;
    } else {
        t2 *= t2;
        n2 = t2 * t2 * grad(gi2, x2, y2, z2);
    }
    float t3 = 0.6f - x3 * x3 - y3 * y3 - z3 * z3;
    if (t3 < 0) {
        n3 = 0.0;
    } else {
        t3 *= t3;
        n3 = t3 * t3 * grad(gi3, x3, y3, z3);
    }
    // Add contributions from each corner to get the final noise value.
    // The result is scaled to stay just inside [-1,1]
    return 32.0f * (n0 + n1 + n2 + n3);
}

// 2D Simplex Noise realization
/**
 * @brief TerrainGenerator::SimplexNoise2D
 * @param x
 * @param z
 * @return 
 */
float TerrainGenerator::SimplexNoise2D(float x, float z)
{
    float n0, n1, n2; // Noise contributions from the three corners

    // Skewing/Unskewing factors for 2D
    static const float F2 = 0.366025403f; // F2 = (sqrt(3) - 1) / 2
    static const float G2 = 0.211324865f; // G2 = (3 - sqrt(3)) / 6   = F2 / (1 + 2 * K)

    // Skew the input space to determine which simplex cell we're in
    const float s = (x + z) * F2; // Hairy factor for 2D
    const float xs = x + s;
    const float ys = z + s;
    const int32_t i = fastfloor(xs);
    const int32_t j = fastfloor(ys);

    // Unskew the cell origin back to (x,y) space
    const float t = static_cast<float>(i + j) * G2;
    const float X0 = i - t;
    const float Y0 = j - t;
    const float x0 = x - X0; // The x,y distances from the cell origin
    const float y0 = z - Y0;

    // For the 2D case, the simplex shape is an equilateral triangle.
    // Determine which simplex we are in.
    int32_t i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
    if (x0 > y0) {  // lower triangle, XY order: (0,0)->(1,0)->(1,1)
	i1 = 1;
	j1 = 0;
    } else { // upper triangle, YX order: (0,0)->(0,1)->(1,1)
	i1 = 0;
	j1 = 1;
    }

    // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
    // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
    // c = (3-sqrt(3))/6

    const float x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
    const float y1 = y0 - j1 + G2;
    const float x2 = x0 - 1.0f + 2.0f * G2; // Offsets for last corner in (x,y) unskewed coords
    const float y2 = y0 - 1.0f + 2.0f * G2;

    // Work out the hashed gradient indices of the three simplex corners
    const int gi0 = permHash(i + permHash(j));
    const int gi1 = permHash(i + i1 + permHash(j + j1));
    const int gi2 = permHash(i + 1 + permHash(j + 1));

    // Calculate the contribution from the first corner
    float t0 = 0.5f - x0 * x0 - y0 * y0;
    if (t0 < 0.0f) {
	n0 = 0.0f;
    } else {
	t0 *= t0;
	n0 = t0 * t0 * grad(gi0, x0, y0);
    }

    // Calculate the contribution from the second corner
    float t1 = 0.5f - x1 * x1 - y1 * y1;
    if (t1 < 0.0f) {
	n1 = 0.0f;
    } else {
	t1 *= t1;
	n1 = t1 * t1 * grad(gi1, x1, y1);
    }

    // Calculate the contribution from the third corner
    float t2 = 0.5f - x2 * x2 - y2 * y2;
    if (t2 < 0.0f) {
	n2 = 0.0f;
    } else {
	t2 *= t2;
	n2 = t2 * t2 * grad(gi2, x2, y2);
    }

    // Add contributions from each corner to get the final noise value.
    // The result is scaled to return values in the interval [-1,1].
    return 45.23065f * (n0 + n1 + n2);
}

// worley noise and its helper functions
/**
 * @brief Create a pseudo-random generated voronoi center point,
 *        for same input, it will generate the same output
 * @return
 */
const glm::vec2 TerrainGenerator :: voronoiCenter(glm::vec2 cornerPos){
    float x = glm::fract(glm::sin(glm::dot(cornerPos, glm::vec2(127.1, 311.7))) * 43758.5453);
    float z = glm::fract(glm::sin(glm::dot(cornerPos, glm::vec2(420.2, 1337.1))) * 789221.1234);

    return glm::vec2(x, z);
}

float TerrainGenerator::worleyNoise(float x, float z)
{
    float intX, fractX;
    fractX = std::modf(x, &intX);

    float intZ, fractZ;
    fractZ = std::modf(z, &intZ);

    float minDist1 = 1;
    float minDist2 = 1;

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            glm::vec2 neighbor = glm::vec2(j, i);
            glm::vec2 point = TerrainGenerator::voronoiCenter(glm::vec2(intX, intZ) + neighbor);
            glm::vec2 diff = neighbor + point - glm::vec2(fractX, fractZ);

            float dist = glm::length(diff);

            if (dist < minDist1) {
                minDist2 = minDist1;
                minDist1 = dist;
            } else if (dist < minDist2) {
                minDist2 = dist;
            }
        }
    }

    return minDist2 - minDist1;
}

/**
 * @brief Given a world coord, return the corresponding blended Height
 * @param x
 * @param z
 * @return the Blended Height
 */
int TerrainGenerator::getHeight(int x, int z)
{
    float grass = getGrassHeight(x, z);
    float mtn = getMountainHeight(x, z);
    float des = getDesertHeight(x, z);

    float perlin = (fbm2D(x/2048.f, z/2048.f, 0.2, PERLIN, 1) + 1) / 2;
    float smoothPerlin = glm::smoothstep(0.5, 0.6, (double) perlin);

    float desPerlin = (fbm2D(x/4096.f, z/4096.f, 0.9, PERLIN, 3) + 1) / 2;
    float desSmoothPerlin = glm::smoothstep(0.7, 0.85, (double) desPerlin);

    return glm::clamp(glm::mix(glm::mix(grass, mtn, smoothPerlin), des, desSmoothPerlin), 0.f, 255.f);
}

/**
 * @brief TerrainGenerator::getGrassHeight
 * @param x
 * @param z
 * @return
 */
int TerrainGenerator::getGrassHeight(float x, float z){
    x /= 512;
    z /= 512;

    int minHeight = 128;
    int maxHeight = 160;
    // use the perturb worley noise
    return minHeight
           + (maxHeight - minHeight)
                 * worleyNoise(fbm2D(x, z, 0.5, PERLIN, 1), fbm2D(z, x, 0.5, PERLIN, 1));
}

/**
 * @brief get the current desert height given the location in the world pos
 * @param x
 * @param z
 * @return the desert height according to the world pos
 */
int TerrainGenerator::getDesertHeight(float x, float z)
{
    x /= 512;
    z /= 512;

    int minHeight = 122;
    int maxHeight = 128;
    return minHeight + (maxHeight - minHeight) * (fbm2D(x, z, 0.5, PERLIN, 3) + 1) / 2;
}

/**
 * @brief TerrainGenerator::getMountainHeight
 * @param x
 * @param z
 * @return
 */
int TerrainGenerator::getMountainHeight(float x, float z)
{
    x /= 4096; // decrease the freq, can be filled in with other values
    z /= 4096; // decrease the freq, can be filed in with other values

    int minHeight = 160;
    int maxHeight = 250;

    return minHeight + (maxHeight - minHeight) * glm::abs(fbm2D(x, z, 0.92, PERLIN, 1));
}

/**
 * @brief TerrainGenerator::isRiver
 * @param x
 * @param y
 * @return 
 */
bool TerrainGenerator::isRiver(float x, float y)
{
    // use the Lsystem generator to define a certain block in the world is river
}

/**
 * @brief Tell if the given block type is empty or not
 * @param x world x
 * @param z world z
 * @param y world y
 * @return true or false for further logic judgement
 */
bool TerrainGenerator::isHole(int x, int z, int y)
{
    // set the threshold
    float value = TerrainGenerator::intervalRemapper(TerrainGenerator::PerlinNoise3D(x / 45.f,
                                                                                     z / 45.f,
                                                                                     y / 45.f),
                                                     -1.f,
                                                     1.f,
                                                     0.f,
                                                     1.f);
    return value > 0.62;
}

/**
 * @brief Terrain::mf_setUpRivers Create an L-system for the river
 * 
 */
void TerrainGenerator::sf_setUpRivers()
{
    // Draw the first river
    // generate river delta first
    srand(time(NULL)); // initilize the random used at the moment
    River RiverDelta(glm::vec2(RIVERMAX / 2, 0),
		     glm::vec2(0.0, 1.f),
		     MARCHSTEP_DELTA,
		     DELTA,
		     "F+-F+-FX");
    RiverDelta.mf_addGrammar('X', "[-FX]+FX");
    RiverDelta.mf_generateSentence(
	MAX_DEPTH_DELTA); // how many times the replacement will take place
    sf_generateRiverMap(&RiverDelta, sp_riverMap.get());
    // Draw the second River that are more straight, like the normal river

    // Initialize a straight River Type
    River RiverStraight(glm::vec2(RIVERMAX / 2, 0),
			glm::vec2(0.0, -1.f),
			MARCHSTEP_STRAIGHT,
			STRAIGHT,
			"+F-F+F-FX");
    // Add grammar to the empty grammar set
    RiverStraight.mf_addGrammar('X', "+F-F+F-FX");
    RiverStraight.mf_generateSentence(MAX_DEPTH_STRAIGHT);
    sf_generateRiverMap(&RiverStraight, sp_riverMap2.get());
}









// TODO : debug session continue
/**
 * @brief Terrain::mf_generateRiverMap
 * @param lsystem
 * @param offsets
 */
void TerrainGenerator::sf_generateRiverMap(LSystem* lsystem, RiverMap* riverMap)
{
    glm::vec2 start;
    glm::vec2 end;
    float zMin; // represents the current stroke's minZ value
    float zMax; // represents the current stroke's minZ
    float xMax; // represents the current stroke's maxX
    float xMin; // represents the current stroke's minX

    float intersectX;
    int depth;
    int offset;
    // initialize the RiverMap
    for (int i = 0; i < RIVERMAX; i++) {
	for (int j = 0; j < RIVERMAX; j++) {
	    (*riverMap)[i][j] = false;
	}
    }

    // loop over LSystem string path
    // starts parsing and render the path
    for (int i = 0; i < lsystem->m_sentence.length(); ++i) {
	// std::cout << "current index at: " << i << " " << std::endl;
	start = lsystem->m_activeTurtle.m_position;
	// Debug:
	// qDebug() << i << ": " << lsystem->m_sentence[i];
	(lsystem->*(lsystem->m_charToDrawingOperation[lsystem->m_sentence[i]]))();

	if (lsystem->m_sentence[i] == 'F') {          // proceed when receiving F
	    end = lsystem->m_activeTurtle.m_position; // set the end of the line
	    LineSegment2D line = LineSegment2D(start, end);
	    // get the four values
	    zMin = start[1] <= end[1] ? start[1] : end[1];
	    zMax = start[1] >= end[1] ? start[1] : end[1];
	    xMin = start[0] <= end[0] ? start[0] : end[0];
	    xMax = start[0] >= end[0] ? start[0] : end[0];

	    // set the river with width
	    // offset for RiverDelta (4, 2, 1, 0)
	    depth = lsystem->m_activeTurtle.m_depth;
	    if (depth == 1) {
		offset = offsets[0];
	    }
	    if (depth == 2) {
		offset = offsets[1];
	    }
	    if (depth == 3) {
		offset = offsets[2];
	    }
	    if (depth > 3) {
		offset = offsets[3];
	    }
	    if (reinterpret_cast<River*>(lsystem)->m_rivertype == STRAIGHT) {
		offset = 12;
	    }
	    // get the upper bound and lower bound  of z value
	    int zUpper = zMax > RIVERMAX - 1 ? RIVERMAX - 1 : ceil(zMax);
	    int zLower = zMin < RIVERMIN ? RIVERMIN : floor(zMin);
	    for (int z = zLower; z <= zUpper; z++) {
		if (line.intersetctAt(z, &intersectX)) { // N/S intersects
		  // define the int value of intersection
		  // get upper bound and lower bound of x value
		  int xInterInt = intersectX < xMin ? xMin : floor(intersectX);
		  int xLower = (xInterInt - offset - random1(-1, 1) < RIVERMIN
				    ? RIVERMIN
				    : xInterInt - offset - random1(-1, 1));
		  int xUpper = (xInterInt + offset + random1(-1, 1) > RIVERMAX - 1
				    ? RIVERMAX - 1
				    : xInterInt + offset + random1(-1, 1));
		  for (int x = xLower; x <= xUpper; x++) {
		    (*riverMap)[z][x] = true; // mark the point as true
		  }
		}
	    }
	}
	// End of the loop : fill out all the table;
    }
    // End of the Whole Sentence parsing
}

/**
 * @brief mf_setUpForests
 */
void TerrainGenerator::sf_setUpTreeBluePrint() {
    Tree tree(glm::vec2{64, 0}, 6.f); // (starting position, march step)
    tree.mf_addGrammar('X', "[-FFFX][+FFFX]");
    tree.mf_generateSentence(TREE_DEPTH);
    sf_generateGrandTreeMap(&tree, sp_treeMap.get());
    ////@Debug:
    //sd_printTreeMap(sp_treeMap.get());
}

/**
 * @brief mf_drawTreeProcedural
 */

/**
 * @brief mf_drawTreeProcedural
 */
void TerrainGenerator::sf_generateGrandTreeMap(LSystem* lsystem, TreeMap* treeMap) {

    glm::vec2 start;
    glm::vec2 end;
    float zMin; // represents the current stroke's minZ value
    float zMax; // represents the current stroke's minZ
    float xMax; // represents the current stroke's maxX
    float xMin; // represents the current stroke's minX

    float intersectX;
    int depth;
    int offset;
    // initialize the RiverMap
    for (int i = 0; i < TREE_MAX; i++) {
	for (int j = 0; j < TREE_MAX; j++) {
	    (*treeMap)[i][j] = false;
	}
    }
    // loop over LSystem string path
    // starts parsing and render the path
    for (int i = 0; i < lsystem->m_sentence.length(); ++i) {
        // std::cout << "current index at: " << i << " " << std::endl;
        start = lsystem->m_activeTurtle.m_position;
        // Debug:
        // qDebug() << i << ": " << lsystem->m_sentence[i];
        (lsystem->*(lsystem->m_charToDrawingOperation[lsystem->m_sentence[i]]))();

	if (lsystem->m_sentence[i] == 'F') {          // proceed when receiving F
	    end = lsystem->m_activeTurtle.m_position; // set the end of the line
	    LineSegment2D line = LineSegment2D(start, end);
	    // get the four values
	    zMin = start[1] <= end[1] ? start[1] : end[1];
	    zMax = start[1] >= end[1] ? start[1] : end[1];
	    xMin = start[0] <= end[0] ? start[0] : end[0];
	    xMax = start[0] >= end[0] ? start[0] : end[0];

	    // set the river with width
	    // offset for RiverDelta (4, 2, 1, 0 )
	    std::array<int, 4> temp_offsets = {10, 7, 5, 3};
	    depth = lsystem->m_activeTurtle.m_depth;
	    if (depth == 1) {
		offset = temp_offsets[0];
	    }
	    if (depth == 2) {
		offset = temp_offsets[1];
	    }
	    if (depth == 3) {
		offset = temp_offsets[2];
	    }
	    if (depth > 3) {
		offset = temp_offsets[3];
	    }
	    // get the upper bound and lower bound  of z value
	    int zUpper = zMax > TREE_MAX - 1 ? TREE_MAX - 1 : ceil(zMax);
	    int zLower = zMin < TREE_MIN ? TREE_MIN : floor(zMin);
	    for (int z = zLower; z <= zUpper; z++) {
		if (line.intersetctAt(z, &intersectX)) { // N/S intersects
		  // define the int value of intersection
		  // get upper bound and lower bound of x value
		  int xInterInt = intersectX < xMin ? xMin : floor(intersectX);
		  int xLower = (xInterInt - offset - random1(-1, 1) < TREE_MIN
				    ? TREE_MIN
				    : xInterInt - offset - random1(-1, 1));
		  int xUpper = (xInterInt + offset + random1(-1, 1) > TREE_MAX - 1
				    ? TREE_MAX - 1
				    : xInterInt + offset + random1(-1, 1));
		  for (int x = xLower; x <= xUpper; x++) {
		    (*treeMap)[z][x] = true; // mark the point as true
		  }
		} else {
		  int xLower = xMin < RIVERMIN ? RIVERMIN : floor(xMin);
		  int xUpper = xMax > RIVERMAX - 1 ? RIVERMAX - 1 : ceil(xMax);
		  for (int x = xLower; x <= xUpper; x++) {
		    // std::cout << x << " " << z << std::endl;
		    (*treeMap)[z][x] = true;
		  }
		}
	    }
	}
	// End of the loop : fill out all the table;
    }
    // End of the Whole Sentence parsing
}

/**
 * @brief mf_drawTreeFixedType1
 */
void TerrainGenerator::sf_drawTreeFixedType1() {}

/**
 * @brief Terrain::md_printRiverMap
 */
void TerrainGenerator::sd_printRiverMap(RiverMap* riverMap) // member function for debug
{
    for (int i = 0; i < 64; i++) {
	std::cout << "==";
    }
    std::cout << std::endl;
    // print the riverMap for debug purpose
    for (int i = 0; i < RIVERMAX; i++) {
	for (int j = 0; j < RIVERMAX; j++) {
	    std::cout << (*riverMap)[i][j];
	}
	std::cout << std::endl;
    }

    for (int i = 0; i < 64; i++) {
	std::cout << "==";
    }
    std::cout << std::endl;
}

void TerrainGenerator::sf_generatorInit()
{
    // initialize any static members
    // make sure that only one instance is initilized
    if (sp_riverMap == nullptr) {
	sp_riverMap = std::make_unique<RiverMap>(); // initialize the rivermap pointer
    } else {
	return;
    }

    if (sp_riverMap2 == nullptr) {
	sp_riverMap2 = std::make_unique<RiverMap>(); // initialize the rivermap pointer
    } else {
	return;
    }

    if(sp_treeMap == nullptr){
        sp_treeMap = std::make_unique<TreeMap>();
    }else{
        return;
    }
    // initialize the Tree map

    // initialize the river map
    sf_setUpRivers();
    // sd_printRiverMap(sp_riverMap.get());
    // sd_printRiverMap(sp_riverMap2.get());
    // initialize the forest map
    sf_setUpTreeBluePrint();

}

/**
 * @brief sd_printTreeMap print the map of the tree for debug
 * @param treeMap the 2D L-system generated map for tree
 */
void TerrainGenerator::sd_printTreeMap(TreeMap* treeMap)
{
    for (int i = 0; i < 64; i++) {
        std::cout << "==";
    }
    std::cout << std::endl;
    // print the riverMap for debug purpose
    for (int i = 0; i < TREE_MAX; i++) {
        for (int j = 0; j < TREE_MAX; j++) {
            std::cout << (*treeMap)[i][j];
        }
        std::cout << std::endl;
    }

    for (int i = 0; i < 64; i++) {
        std::cout << "==";
    }
    std::cout << std::endl;
}
