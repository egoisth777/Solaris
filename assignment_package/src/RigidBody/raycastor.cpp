#include "raycastor.h"
#include <iostream>

Ray RayCastor::m_ray = Ray();

void RayCastor::generateRay(glm::vec3 origin, glm::vec3 direction, float length)
{
    m_ray.rayDirection = direction;
    m_ray.rayLength = length;
    m_ray.rayOrigin = origin;
    m_ray.rayTargetPos = origin + length * direction;
}

std::optional<HitInfo>
RayCastor::castRayToValidTarrain(const Terrain &terrain, const Ray &ray)
{
    HitInfo result;
    bool getHit = false;

    try{
        getHit = gridMarch(ray.rayOrigin,
                                ray.rayDirection * ray.rayLength,
                                terrain,
                                &result.dist,
                                &result.hitCubeIndex);
    }
    catch (std::out_of_range& e){
        std::cerr << e.what() << std::endl;
    }

    if(getHit){
        result.hitPosition = ray.rayOrigin + ray.rayDirection * result.dist;
        return {result};
    }

    return {};
}


bool RayCastor::gridMarch(glm::vec3 rayOrigin,
                          glm::vec3 rayDirection,
                          const Terrain &terrain,
                          float *out_dist,
                          glm::ivec3 *out_blockHit)
{
    float maxLen = glm::length(rayDirection); // Farthest we search
    glm::ivec3 currCell = glm::ivec3(glm::floor(rayOrigin));
    rayDirection = glm::normalize(rayDirection); // Now all t values represent world dist.
#if 0
    /// Return 0 when exactly touching a non-empty block
    for(int i = 0; i < 3; ++i) { // Iterate over the three axes
        if(rayDirection[i] == 0) { // Is ray parallel to axis i?
            continue;
        }

        if(currCell[i] != rayOrigin[i]){
            continue;
        }

        int offset = glm::min(0.0f, glm::sign(rayDirection[i]));
        glm::ivec3 interfaceCell = currCell;
        interfaceCell[i] += offset;
        BlockType cellType = terrain.getBlockAt(interfaceCell.x, interfaceCell.y, interfaceCell.z);
        if(cellType == EMPTY){
            continue;
        }

        *out_blockHit = interfaceCell;
        *out_dist = 0;
        return true;
    }
#endif
    float curr_t = 0.f;
    while(curr_t < maxLen) {
        float min_t = glm::sqrt(3.f);
        float interfaceAxis = -1; // Track axis for which t is smallest
        for(int i = 0; i < 3; ++i) { // Iterate over the three axes
            if(rayDirection[i] == 0) { // Is ray parallel to axis i?
                continue;
            }
            float offset = glm::max(0.f, glm::sign(rayDirection[i]));
            // If the player is *exactly* on an interface then
            // they'll never move if they're looking in a negative direction
            if(currCell[i] == rayOrigin[i] && offset == 0.f) {
                offset = -1.f;
            }
            int nextIntercept = currCell[i] + offset;
            float axis_t = (nextIntercept - rayOrigin[i]) / rayDirection[i];
            axis_t = glm::min(axis_t, maxLen); // Clamp to max len to avoid super out of bounds errors
            if(axis_t < min_t) {
                min_t = axis_t;
                interfaceAxis = i;
            }
        }
        if(interfaceAxis == -1) {
            throw std::out_of_range("interfaceAxis was -1 after the for loop in gridMarch!");
        }
        curr_t += min_t;
        rayOrigin += rayDirection * min_t;
        glm::ivec3 offset = glm::ivec3(0,0,0);
        // Sets it to 0 if sign is +, -1 if sign is -
        offset[interfaceAxis] = glm::min(0.f, glm::sign(rayDirection[interfaceAxis]));
        currCell = glm::ivec3(glm::floor(rayOrigin)) + offset;
        // If currCell contains something other than EMPTY, return curr_t
        BlockType cellType = EMPTY;
        try{
            cellType = terrain.getBlockAt(currCell.x, currCell.y, currCell.z);
        }
        catch(std::out_of_range&){
            //  cellType = EMPTY;
            /// Do nothing when out of bound, just treat it as EMPTY
        }

        if(isCollider(cellType)) {
            *out_blockHit = currCell;
            *out_dist = glm::min(maxLen, curr_t);
            return true;
        }
    }
    *out_dist = glm::min(maxLen, curr_t);
    return false;
}
