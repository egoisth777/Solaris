#include "river.h"

River::River(glm::vec2 pos, glm::vec2 heading, float fDistance, RIVERTYPE direction, QString axiom)
    : LSystem(pos, heading, fDistance, axiom)
    , m_rivertype(direction)
{
  m_branchProb = m_rivertype == DELTA ? 0.8 : 1.0; // more frequent branching event
}

River::~River() {}

void River::pushStack()
{
  Turtle turtleCopy(m_activeTurtle);
  m_turtleStack.push(turtleCopy);
  m_activeTurtle.increaseDepth();
}

void River::rotateLeft()
{
  m_activeTurtle.turnLeft(PI_8 - TerrainGenerator::randAngle());
  if (this->m_rivertype == DELTA) {
    if (m_activeTurtle.m_orientation[1] < 0) {
      m_activeTurtle.m_orientation = glm::vec2(0, 1.f); // move towards north
    }
  } else {
    if (m_activeTurtle.m_orientation[1] < 0) {
      m_activeTurtle.m_orientation = glm::vec2(0.f, 1.f); // move towards South
    }
  }
}

void River::rotateRight()
{
  m_activeTurtle.turnRight(-PI_8 + TerrainGenerator::randAngle());
  if (this->m_rivertype == DELTA) {
    if (m_activeTurtle.m_orientation[1] < 0) {
      m_activeTurtle.m_orientation = glm::vec2(0.f, 1.f);
    }
  } else {
    if (m_activeTurtle.m_orientation[1] < 0) {
      m_activeTurtle.m_orientation = glm::vec2(0.f, 1.f); // keep meaandering south
    }
  }
}

/**
* @brief mf_generateSentence overriden sentence generator using branching probability
*        Note the iterator here applies to situation where there's only one rule in the rule set
*        For Grammar set more than 2, this function will break, and generate in-correct result
* @param n
*/
void River::mf_generateSentence(int n)
{
  QString s = m_axiom; // set the beginning string
  float r;
  for (int i = 0; i < n; i++) { // perform the traversal n times
    QHashIterator<QChar, QString> iter(m_grammarSet);
    // BEGIN OF WHILE
    while (iter.hasNext()) { // check for each rule
      iter.next();
      r = glm::linearRand(0.f, 1.f);
      if (r > m_branchProb || this->m_rivertype == STRAIGHT) {
	s.replace(iter.key(),
		  iter.value()); // replace all the occurences of iter.key() with iter.value
      } else {
	s.replace(iter.key(), NON_BRANCHING_GRAMMAR);
      }
    }
    // END OF WHILE
  }
  m_sentence = s;
}
