#ifndef RIVER_H
#define RIVER_H
#include "lsystem.h"
#include "terraingenerator.h"
#include <glm/gtc/random.hpp>

enum RIVERTYPE { DELTA, STRAIGHT };
const QString NON_BRANCHING_GRAMMAR = "+--+FX";

class River : public LSystem
{
public:
  River(glm::vec2 pos, glm::vec2 heading, float fDistance, RIVERTYPE direction, QString axiom);
  ~River();

public: // getters and setters (for debug only)
  inline void mf_setDirection(RIVERTYPE direction) { this->m_rivertype = direction; }
  inline RIVERTYPE mf_getDirection() const { return this->m_rivertype; }

public: // override few commands to customize the generations
  void mf_generateSentence(int n) override;
  void pushStack() override;
  void rotateLeft() override;
  void rotateRight() override;

public: // determines where the river runs
  RIVERTYPE m_rivertype;
};

#endif RIVER_H
