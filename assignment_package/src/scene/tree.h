#ifndef TREE_H
#define TREE_H

#include "lsystem.h"

class Tree : public LSystem
{
public:
  Tree(glm::vec2 pos, float fDistance);
  virtual ~Tree() = default;
};

#endif // TREE_H
