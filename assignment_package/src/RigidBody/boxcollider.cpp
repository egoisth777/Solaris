#include "boxcollider.h"
#include "Enums/directionenums.h"
#include "RigidBody/raycastor.h"
#include <iostream>
#include "scene/entity.h"

#define BOX_THICK 0.0001f

BoxCollider::BoxCollider(Entity* entity, const Terrain & terrain,
                         glm::vec3 boxSize, glm::vec3 posOffset) :
    IGameComponent(entity),
    m_boxSize(boxSize),
    m_origin(posOffset),
    mcr_terrain(terrain)
{
    std::unordered_map<Direction, glm::vec3> faceCenters;
    faceCenters.reserve(6);
    faceCenters = {
        {Direction::XPOS, posOffset + boxSize.x * glm::vec3(1, 0, 0)},
        {Direction::XNEG, posOffset - boxSize.x * glm::vec3(1, 0, 0)},
        {Direction::YPOS, posOffset + boxSize.y * glm::vec3(0, 1, 0)},
        {Direction::YNEG, posOffset - boxSize.y * glm::vec3(0, 1, 0)},
        {Direction::ZPOS, posOffset + boxSize.z * glm::vec3(0, 0, 1)},
        {Direction::ZNEG, posOffset - boxSize.z * glm::vec3(0, 0, 1)}
    };

    std::unordered_map<Direction, std::array<glm::vec3, 4>> faceCenterOffsets;
    faceCenterOffsets.reserve(6);
    std::array<glm::vec3, 4> xOffset = {
        boxSize * glm::vec3(0, 1, 1),
        boxSize * glm::vec3(0, -1, 1),
        boxSize * glm::vec3(0, 1, -1),
        boxSize * glm::vec3(0, -1, -1),
    };
    std::array<glm::vec3, 4> yOffset = {
        boxSize * glm::vec3(1, 0, 1),
        boxSize * glm::vec3(-1, 0,  1),
        boxSize * glm::vec3(1,  0, -1),
        boxSize * glm::vec3(-1, 0,  -1),
    };
    std::array<glm::vec3, 4> zOffset = {
        boxSize * glm::vec3(1, 1, 0),
        boxSize * glm::vec3(-1, 1, 0),
        boxSize * glm::vec3(1, -1, 0),
        boxSize * glm::vec3(-1, -1, 0),
    };
    faceCenterOffsets = {
        {Direction::XPOS, xOffset},
        {Direction::XNEG, xOffset},
        {Direction::YPOS, yOffset},
        {Direction::YNEG, yOffset},
        {Direction::ZPOS, zOffset},
        {Direction::ZNEG, zOffset}
    };

    for(const auto& p : faceCenters){
        Direction dir = p.first;
        std::array<glm::vec3, 4> faceVertices(faceCenterOffsets[dir]);
        //std::cout << std::to_string(dir) << std::endl;
        for(unsigned int i = 0; i < faceVertices.size(); ++i){
            faceVertices[i] += p.second;
            //std::cout << faceVertices[i].x << ' ' << faceVertices[i].y << ' ' << faceVertices[i].z << std::endl;
        }

        faces[dir] = faceVertices;
    }

    collidingDirection.reserve(6);
}

glm::vec3 BoxCollider::boxRayCast(const glm::vec3 &dir)
{
    collidingDirection.clear();

    std::array<Direction, 3> movingDir = {
        dir.x > 0 ? Direction::XPOS : Direction::XNEG,
        dir.y > 0 ? Direction::YPOS : Direction::YNEG,
        dir.z > 0 ? Direction::ZPOS : Direction::ZNEG
    };

    glm::vec3 result(0);
    for(unsigned int i = 0; i < movingDir.size(); ++i){
        result[i]=   glm::sign(dir[i]) *
                    boxRayCastOnDir(movingDir[i], glm::abs(dir[i]));
    }

    return result;
}

bool BoxCollider::isCollidingOnDir(Direction dir)
{
    return collidingDirection.find(dir) != collidingDirection.end();
}

bool BoxCollider::isCollideInside() const
{

    return false;
}

bool BoxCollider::isInCubePosition(glm::ivec3 coord) const
{
    glm::vec3 upperCorner = glm::floor(m_origin + m_boxSize + m_gameObject->mcr_position);
    glm::vec3 lowerCorner = glm::floor(m_origin - m_boxSize + m_gameObject->mcr_position);
    return coord.x >= lowerCorner.x && coord.x <= upperCorner.x &&
           coord.y >= lowerCorner.y && coord.y <= upperCorner.y &&
           coord.z >= lowerCorner.z && coord.z <= upperCorner.z;
}

float BoxCollider::boxRayCastOnDir(Direction dir, float length)
{
    float resultLength = length;

    for(auto& vertex : faces[dir]){
        float dist = boxRayCastVertex(dir, vertex, length);
        resultLength = glm::min(dist, resultLength);
    }
    if(glm::abs(resultLength) < BOX_THICK){
        collidingDirection.insert(dir);
    }
    //std::cout <<(int)dir << ' ' << resultLength << ' ' << collidingTable[dir] << std::endl;

    return resultLength;
}

float BoxCollider::boxRayCastVertex(Direction dir, glm::vec3 vertex, float length)
{
    glm::vec3 pos = getGameObject()->mcr_position + vertex;
    RayCastor::generateRay(pos, DirectionToVec[dir], length);
    std::optional<HitInfo> result = RayCastor::castRayToValidTarrain(mcr_terrain);

    if(!result){
        return length;
    }

    /// When the box touch some null-empty surface exacly,
    /// All three directions' ray will be blocked by that surface.
    /// Hence, use a small offset to prevent exact surface touch
    float dist = result.value().dist - BOX_THICK;

    return dist;
}


#if 0
float BoxCollider::boxRayCastArray(Direction dir,
                                   glm::vec3 leftCorner,
                                   glm::vec3 rightCorner,
                                   float length)
{
    glm::vec3 updatingAxis = DirectionToVec[dir];
    updatingAxis = -glm::sign(updatingAxis) * updatingAxis + glm::vec3(1);
    std::array<int, 2> axis =
        updatingAxis.x == 0 ? std::array<int, 2>{1, 2} :
        updatingAxis.y == 0 ? std::array<int, 2>{0, 2} :
        std::array<int, 2>{0, 1} ;

    glm::vec2 boundaryX = glm::vec2(
        glm::min(leftCorner[axis[0]], rightCorner[axis[0]]),
        glm::max(leftCorner[axis[0]], rightCorner[axis[0]]));
    glm::vec2 boundaryY = glm::vec2(
        glm::min(leftCorner[axis[1]], rightCorner[axis[1]]),
        glm::max(leftCorner[axis[1]], rightCorner[axis[1]]));
    for(float x = boundaryX[0]; x <= boundaryX[1]; x += 1){
        for(float y = boundaryY[0]; y <= boundaryY[1]; y += 1){

        }
    }
}
#endif
