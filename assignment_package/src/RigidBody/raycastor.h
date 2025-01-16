#ifndef RAYCASTOR_H
#define RAYCASTOR_H

#include "glm_includes.h"

#include <scene/terrain.h>

struct Ray
{
public:
    glm::vec3 rayOrigin;
    glm::vec3 rayDirection;
    glm::vec3 rayTargetPos;
    float rayLength;
    Ray() :
        Ray(glm::vec3(0), glm::vec3(0), glm::vec3(0), 0)
    {}

    Ray(glm::vec3 origin, glm::vec3 direction, glm::vec3 targetPos, float length) :
        rayOrigin(origin),
        rayDirection(direction),
        rayTargetPos(targetPos),
        rayLength(length)
    {}
};

struct HitInfo{
public:
    float dist;
    glm::vec3 hitPosition;
    glm::ivec3 hitCubeIndex;
};


class RayCastor{
private:
    static Ray m_ray;

public:
    static void generateRay(glm::vec3 origin, glm::vec3 direction, float length);
    static std::optional<HitInfo>
    castRayToValidTarrain(const Terrain &, const Ray& = m_ray);

private:
    static bool gridMarch(glm::vec3 rayOrigin,
                          glm::vec3 rayDirection,
                          const Terrain &terrain,
                          float *out_dist,
                          glm::ivec3 *out_blockHit);
};

#endif // RAYCASTOR_H
