#include "lsystem.h"

///////////////////////////////////////////////////////////////////
/// Turtle class implementation
///
///
///
///
///
///
///
///////////////////////////////////////////////////////////////////

/**
 * @brief Turtle::Turtle
 */
Turtle::Turtle()
    : m_position(glm::vec2(0, 0))
    , m_orientation(glm::vec2(0, 1))
    , m_depth(1)
    , m_fDistance(1.0f)
{}

/**
 * @brief Turtle::Turtle
 * @param position
 * @param orientation
 * @param fDistance
 */
Turtle::Turtle(glm::vec2 position, glm::vec2 orientation, float fDistance)
    : m_position(position)
    , m_orientation(orientation)
    , m_depth(1)
    , m_fDistance(fDistance)
{}

/**
 * Copy Constructor
 * @brief Turtle::Turtle
 * @param t
 */
Turtle::Turtle(const Turtle &t)
    : m_position(t.m_position)
    , m_orientation(t.m_orientation)
    , m_depth(t.m_depth)
    , m_fDistance(t.m_fDistance)
{}

/**
 * @brief Turtle::~Turtle
 */
Turtle::~Turtle() {}

/**
 * @brief Rewrite of the assignment operator
 * @param other
 * @return 
 */
Turtle &Turtle::operator=(const Turtle &other)
{
  this->m_position = other.m_position;
  this->m_orientation = other.m_orientation;
  this->m_fDistance = other.m_fDistance;
  this->m_depth = other.m_depth;

  return *this;
}

void Turtle::turnLeft(float angle)
{
  this->m_orientation = glm::vec2(cosf(angle) * m_orientation.x - sinf(angle) * m_orientation.y,
				  sinf(angle) * m_orientation.x + cosf(angle) * m_orientation.y);
}

void Turtle::turnRight(float angle)
{
  this->m_orientation = glm::vec2(cosf(angle) * m_orientation.x - sinf(angle) * m_orientation.y,
				  sinf(angle) * m_orientation.x + cosf(angle) * m_orientation.y);
}

void Turtle::moveForward()
{
  m_position += m_orientation * m_fDistance;
}

void Turtle::increaseDepth()
{
  m_depth++;
}
void Turtle::decreaseDepth()
{
  m_depth--;
}

void Turtle::printCoordinates() const
{
  qDebug() << "(" << m_position.x << " , " << m_position.y << ")";
}

void Turtle::printOrientation() const
{
  qDebug() << "(" << m_orientation.x << " , " << m_orientation.y << ")";
}

///////////////////////////////////////////////////////////////////
/// SPECIAL MARKER
/// Lsystem class implementation
///
///
///
///
///
///
///////////////////////////////////////////////////////////////////

// initializer and setter

/**
 * @brief LSystem::LSystem
 * @param pos
 * @param heading
 * @param fDistance
 */
LSystem::LSystem(glm::vec2 pos, glm::vec2 heading, float fDistance, QString axiom)
    : m_axiom(axiom)
    , m_sentence("")
    , m_activeTurtle(pos, heading, fDistance)
    , m_branchProb(0.9f)
{
  mf_populateOperations();
}

/**
 * @brief LSystem::~LSystem
 */
LSystem::~LSystem()
{
  //qDebug() << "L-system deleted";
}

/**
 * @brief LSystem::mf_generatePath | generate path based on the axiom
 * @param n how many iterations will run the apply rule
 * @param seed
 */
void LSystem::mf_generateSentence(int n)
{
    // check if the grammar set is filled
    if (m_grammarSet.isEmpty()) {
      qDebug() << "Grammar set is empty, make sure to add rules before generating path";
      return;
    }

    // string generation (Apply the replacement rule for n times)
    QString s = m_axiom;
    for (int i = 0; i < n; i++) {
        QHashIterator<QChar, QString> j(m_grammarSet);
        while (j.hasNext()) {
            j.next();
            s.replace(QRegularExpression(QString(j.key())), j.value());
        }
    }
    m_sentence = s;
}

/**
 * @brief LSystem::mf_populateOperations populate the operations with the function pointers
 */
void LSystem::mf_populateOperations()
{
    m_charToDrawingOperation['['] = &LSystem::pushStack;
    m_charToDrawingOperation[']'] = &LSystem::popStack;
    m_charToDrawingOperation['F'] = &LSystem::moveForward;
    m_charToDrawingOperation['-'] = &LSystem::rotateLeft;
    m_charToDrawingOperation['+'] = &LSystem::rotateRight;
    m_charToDrawingOperation['X'] = &LSystem::XCommand;
}

/**
 * @brief LSystem::mf_printPath
 */
void LSystem::mf_printPath()
{
    qDebug() << m_sentence.toUtf8().constData();
}

/**
 * @brief LSystem::mf_addGrammar add a grammar to the grammar set
 * @param character
 * @param string
 */
void LSystem::mf_addGrammar(QChar character, QString string)
{
    m_grammarSet[character] = string;
}

/**
 * @brief push the current active turtle into the stack 
 * Basically save the current context of the painting
 */
void LSystem::pushStack()
{
    m_activeTurtle.increaseDepth();
    Turtle turtleCopy(m_activeTurtle);
    this->m_turtleStack.push(turtleCopy);
}
/**
 * @brief LSystem::popStack
 * Pop the most upwards turtle in the stack, which restores back to the 
 * last saved position and orientation
 */
void LSystem::popStack()
{
    m_activeTurtle = m_turtleStack.pop();
    m_activeTurtle.decreaseDepth();
}
void LSystem::moveForward()
{
    this->m_activeTurtle.moveForward();
}

void LSystem::rotateLeft()
{
    this->m_activeTurtle.turnLeft(M_PI / 6.0);
}
void LSystem::rotateRight()
{
    this->m_activeTurtle.turnRight(-M_PI / 6.0);
}

void LSystem::XCommand()
{
    return;
}

///////////////////////////////////////////////////////////////////
/// END OF THE IMPLEMENTATION
///////////////////////////////////////////////////////////////////
