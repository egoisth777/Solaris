#include "entity.h"

Entity::Entity()
    :  Entity(glm::vec3(0,0,0))
{}

Entity::Entity(glm::vec3 pos)
    : m_forward(0,0,-1), m_right(1,0,0), m_up(0,1,0), m_position(pos), mcr_position(m_position)
{}

Entity::Entity(const Entity &e)
    : m_forward(e.m_forward), m_right(e.m_right), m_up(e.m_up), m_position(e.m_position), mcr_position(m_position)
{}

MovingState Entity::getMovingState() const
{
    return m_movingState;
}

bool Entity::isUnderMovingState(MEntity::EntityMovingState state) const
{
    return MEntity::isUnderState(getMovingState(), state);
}

void Entity::setPosition(glm::vec3 val)
{
    m_position = val;
}

glm::vec3 Entity::mapLocalAttribToGlobal(glm::vec3 dir)
{
    return glm::normalize(dir.z * m_forward + dir.y * m_up + dir.x * m_right);
}

Entity::~Entity()
{}

glm::vec3 Entity::getForward() const
{
    return m_forward;
}

glm::vec3 Entity::getRight() const
{
    return m_right;
}

glm::vec3 Entity::getUp() const
{
    return m_up;
}

std::vector<uPtr<IGameComponent>>& Entity::getComponents()
{
    return this->m_components;
}

void Entity::moveAlongVector(glm::vec3 dir) {
    m_position += dir;
}

void Entity::moveForwardLocal(float amount) {
    m_position += amount * m_forward;
}

void Entity::moveRightLocal(float amount) {
    m_position += amount * m_right;
}

void Entity::moveUpLocal(float amount) {
    m_position += amount * m_up;
}

void Entity::moveForwardGlobal(float amount) {
    m_position += glm::vec3(0, 0, amount);
}

void Entity::moveRightGlobal(float amount) {
    m_position += glm::vec3(amount, 0, 0);
}

void Entity::moveUpGlobal(float amount) {
    m_position += glm::vec3(0, amount, 0);
}

void Entity::rotateOnForwardLocal(float degrees) {
    float rad = glm::radians(degrees);
    glm::mat4 rot(glm::rotate(glm::mat4(), rad, m_forward));
    m_right = glm::vec3(rot * glm::vec4(m_right, 0.f));
    m_up = glm::vec3(rot * glm::vec4(m_up, 0.f));
}

void Entity::rotateOnRightLocal(float degrees) {
    float rad = glm::radians(degrees);
    glm::mat4 rot(glm::rotate(glm::mat4(), rad, m_right));
    m_forward = glm::vec3(rot * glm::vec4(m_forward, 0.f));
    m_up = glm::vec3(rot * glm::vec4(m_up, 0.f));
}

void Entity::rotateOnUpLocal(float degrees) {
    float rad = glm::radians(degrees);
    glm::mat4 rot(glm::rotate(glm::mat4(), rad, m_up));
    m_right = glm::vec3(rot * glm::vec4(m_right, 0.f));
    m_forward = glm::vec3(rot * glm::vec4(m_forward, 0.f));
}


void Entity::rotateOnForwardGlobal(float degrees) {
    float rad = glm::radians(degrees);
    m_forward = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(0,0,1)) * glm::vec4(m_forward, 0.f));
    m_right = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(0,0,1)) * glm::vec4(m_right, 0.f));
    m_up = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(0,0,1)) * glm::vec4(m_up, 0.f));
}

void Entity::rotateOnRightGlobal(float degrees) {
    float rad = glm::radians(degrees);
    m_forward = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(1,0,0)) * glm::vec4(m_forward, 0.f));
    m_right = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(1,0,0)) * glm::vec4(m_right, 0.f));
    m_up = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(1,0,0)) * glm::vec4(m_up, 0.f));
}

void Entity::rotateOnUpGlobal(float degrees) {
    float rad = glm::radians(degrees);
    m_forward = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(0,1,0)) * glm::vec4(m_forward, 0.f));
    m_right = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(0,1,0)) * glm::vec4(m_right, 0.f));
    m_up = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(0,1,0)) * glm::vec4(m_up, 0.f));
}

void Entity::switchMovingMode(MovingState state)
{
    MEntity::switchMask(this->m_movingState, state);
    onMovingStateChangeTo(state, MEntity::isUnderState(this->m_movingState, state));
}

void Entity::switchMovingMode(MEntity::EntityMovingState target, bool toTarget)
{
    if(isUnderState(this->m_movingState, target) == toTarget){
        return;
    }
    else{
        switchMovingMode(target);
    }
}

MovingState &Entity::getMovingState()
{
    return this->m_movingState;
}

void Entity::onMovingStateChangeTo(MovingState, bool)
{
    return;
}
