#ifndef LSYSTEM_H
#define LSYSTEM_H
#include <QDebug>
#include <QHash>
#include <QRegularExpression>
#include <QStack>
#include <QString>
#include "terraingenerator.h"
#include <glm_includes.h>

const float PI_3 = M_PI / 3.0;
const float PI_4 = M_PI / 4.0;
const float PI_6 = M_PI / 6.0;
const float PI_8 = M_PI / 8.0;
#define INF 9999999

struct LineSegment2D
{
  glm::vec2 m_start;
  glm::vec2 m_end;
  float m_slope;

  LineSegment2D(glm::vec2 &start, glm::vec2 &end)
      : m_start(start)
      , m_end(end)
      , m_slope((end.y - start.y == 0 ? INF : (end.y - start.y) / (end.x - start.x)))
  {}

  ~LineSegment2D() {}

  /**
     * @brief Judge whether the line intersects with the z = y_row
     *        return the x_Intersection ni xIntersect, 
     *        return whether if the horizontal z = y_row and this(line) 
     *        intersects with the return value bool
     * @param y_row 
     * @param xIntersect
     * @return whether the line intersects with the row or not
     */
  bool intersetctAt(const int y_row, float *xIntersect)
  {
    float y = (float) y_row;
    // no intersection if y is outside of the boundary of the vertical line
    float yUpperBound = m_end[1] > m_start[1] ? m_end[1] : m_start[1];
    float yLowerBound = m_end[1] < m_start[1] ? m_end[1] : m_start[1];

    if (y > yUpperBound || y < yLowerBound) {
      return false;
    }
    if (m_slope == 0) {
      return false;
    }

    if (m_start[0] == m_end[0]) {
      *xIntersect = m_start[0];
      return true;
    }

    float x = (y - m_start[1]) / m_slope + m_start[0];
    *xIntersect = x;
    return true;
  }
};

class LSystem;
typedef void (LSystem::*Rule)(void); // define a function poiter

/**
 * @brief The Turtle class
 */
class Turtle
{
public: // constructors
  Turtle();
  Turtle(glm::vec2 pos, glm::vec2 heading, float fDistance);
  Turtle(const Turtle &t);
  ~Turtle();

public: // member functions
  void turnLeft(float angle);
  void turnRight(float angle);
  void moveForward();
  void increaseDepth();
  void decreaseDepth();

  void printCoordinates() const; // prints turtle's position
  void printOrientation() const; // prints turtle's orientation vector

  Turtle &operator=(const Turtle &other);

private:
  float randAngle() const; // returns an angle between 0 and PI/4 radians

public:
  glm::vec2 m_position;    // starting position of turtle
  glm::vec2 m_orientation; // staring orientation of turtle
  int m_depth;             // recursion depth of the turtle
  float m_fDistance;       // distance turtle moves on 'F
};

/**
 * @brief The LSystem class
 * Define the basic LSystem component, while emitting the 
 * details of the specific grammar
 */
class LSystem
{
public: // initializers and setters
  LSystem(glm::vec2 pos, glm::vec2 heading, float fDistance, QString axiom);
  virtual ~LSystem();
  virtual void mf_populateOperations();

public: // testers
  void mf_printPath();

protected:
  void mf_addGrammar(QChar character, QString string);
  virtual void mf_generateSentence(int n); // remember to call addRules before calling generate Path
  // possibly be rewritten for sub class requirements

  virtual void pushStack();
  virtual void popStack();
  virtual void moveForward();
  virtual void rotateLeft();
  virtual void rotateRight();
  virtual void XCommand();

protected: // member variables
  QString m_axiom;
  QString m_sentence;
  Turtle m_activeTurtle;
  QStack<Turtle> m_turtleStack;
  QHash<QChar, QString> m_grammarSet;
  QHash<QChar, Rule> m_charToDrawingOperation;
  float m_branchProb;

  // declare some friend classes that can use the function
  friend class Terrain;
  friend class TerrainGenerator;
};

#endif // LSYSTEM_H
