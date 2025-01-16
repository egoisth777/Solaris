#ifndef DIRECTIONENUMS_H
#define DIRECTIONENUMS_H

#include "glm_includes.h"
#include <array>

// Direction is defined in "scene/chunk.h", will move it here after merging
// The mapping is based on the value of the Direction, so be careful when changing its value
const std::array<glm::vec3, 6> DirectionToVec = {
    glm::vec3(0, 0, 1),
    glm::vec3(1, 0, 0),
    glm::vec3(-1, 0, 0),
    glm::vec3(0, 0, -1),
    glm::vec3(0, 1, 0),
    glm::vec3(0, -1, 0)
};



#endif // DIRECTIONENUMS_H
