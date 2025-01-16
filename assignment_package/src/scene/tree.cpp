#include "tree.h"

/**
 * @brief constructor with parameters 
 * @param pos
 * @param fDistance
 */
Tree::Tree(glm::vec2 pos, float fDistance)
    : LSystem(pos, glm::vec2(0.f, 1.f), fDistance, "FFFFFFFFFFX")
{}

