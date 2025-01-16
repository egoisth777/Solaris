#include "renderutils.h"
#include <cstdlib>
#include <iostream>

RenderUtils::RenderUtils()
{}

std::vector<glm::vec2> RenderUtils::generateSSAONoise(int count)
{
    std::vector<glm::vec2> result;
    result.reserve(count);
    for(int i = 0; i < count; ++i){
        result.push_back(glm::vec2(rand(-1, 1), rand(-1, 1)));
    }
    return result;
}

std::vector<glm::vec3> RenderUtils::generateSSAOSamplePoints(int count)
{

    std::vector<glm::vec3> result;
    result.reserve(count);

    /// Generate hemi-sphere sampling point
    for(int i = 0; i < count; ++i){

        glm::vec3 point = glm::vec3(rand(-1, 1), rand(-1, 1), rand(0, 1));

        glm::normalize(point);
        point *= rand(0, 1);
        float scale = (float) i / (float) count;
        scale = glm::mix(0.1, 1.0, scale * scale);
        point *= scale;

        result.push_back(point);
    }
    return result;
}

float RenderUtils::rand(float lower, float upper)
{
    return lower + (float)std::rand() / double(RAND_MAX) * (upper - lower);
}

glm::vec3 RenderUtils::rand3f(float lower, float upper)
{
    return glm::vec3(rand(lower, upper), rand(lower, upper), rand(lower, upper));
}
