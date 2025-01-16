#ifndef RENDERUTILS_H
#define RENDERUTILS_H

#include <vector>
#include <glm_includes.h>



class RenderUtils
{
public:
    RenderUtils();

    // Static helper function
public:
    static std::vector<glm::vec2> generateSSAONoise(int count);
    static std::vector<glm::vec3> generateSSAOSamplePoints(int count);

    /// return float in [lower, upper]
    static inline float rand(float lower, float upper);

    /// return a vec3 with xyz in [lower, upper]
    static inline glm::vec3 rand3f(float lower, float upper);
};

#endif // RENDERUTILS_H
