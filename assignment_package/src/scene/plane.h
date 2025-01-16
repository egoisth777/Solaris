#ifndef PLANE_H
#define PLANE_H
#include "glm_includes.h"

struct Plane{
    glm::vec3 normal;
    glm::vec3 pointOnPlane;

    Plane(glm::vec3 norm = glm::vec3(0), glm::vec3 pt = glm::vec3(0)) :
        normal(norm),
        pointOnPlane(pt)
    {};

    float getSignedDistanceToPlane(const glm::vec3& point) const
    {
        return glm::dot(normal, point - pointOnPlane);
    }
};

#endif // PLANE_H
