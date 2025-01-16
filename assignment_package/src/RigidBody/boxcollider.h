#ifndef BOXCOLLIDER_H
#define BOXCOLLIDER_H

#include <Interface/igamecomponent.h>
#include <scene/terrain.h>
#include <unordered_map>
#include "glm_includes.h"
#include "scene/chunk.h"

class BoxCollider : public IGameComponent
{
private:
    const glm::vec3 m_boxSize;
    const glm::vec3 m_origin;
    const Terrain & mcr_terrain;

    // Offsets of each face relative to the pivot of the game entity
    std::unordered_map<Direction, std::array<glm::vec3, 4>> faces;
    std::unordered_set<Direction> collidingDirection;

public:

    /// Define a collider size and offset relative to its entity's pivot
    /// The result will be a box with size 2 * boxSize (positive & negative),
    /// and shifted by boxOffset
    BoxCollider(Entity*, const Terrain &, glm::vec3 boxSize, glm::vec3 boxOffset);

    /// Given a direction, represent the destination in global coord
    /// apply raycast in proper face to find max-reachable position in global coord
    glm::vec3 boxRayCast(const glm::vec3& dir);

    bool isCollidingOnDir(Direction);

    /// Return true iff the collider is overlapping with some collidable entity
    bool isCollideInside() const;

    /// Check if a cube specified by ivec3 in world is overlapped with this collider
    bool isInCubePosition(glm::ivec3) const;

private:

    /// Cast ray on direction and return the min dist to reach the terrain
    float boxRayCastOnDir(Direction, float length);

    //float boxRayCastArray(Direction, glm::vec3, glm::vec3, float length);
    float boxRayCastVertex(Direction, glm::vec3, float length);
};

#endif // BOXCOLLIDER_H
