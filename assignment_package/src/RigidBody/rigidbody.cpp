#include "rigidbody.h"
#include "Enums/directionenums.h"
#include "scene/player.h"
#include <globalinstance.h>
#include <iostream>

glm::vec3 RigidBody::velocity() const
{
    return m_velocity;
}

void RigidBody::setVelocity(const glm::vec3 &newVelocity)
{
    m_velocity = newVelocity;
}

glm::vec3 RigidBody::targetVelocityFactor() const
{
    return m_targetVelocityFactor;
}

void RigidBody::setTargetVelocityFactor(const glm::vec3 &newAcceleration)
{
    m_targetVelocityFactor = newAcceleration;
}

bool RigidBody::isGravity() const
{
    return m_isGravity;
}

void RigidBody::setIsGravity(bool newIsGravity)
{
    m_isGravity = newIsGravity;
}

glm::vec3 RigidBody::velocityMax() const
{
    return m_velocityMax;
}

void RigidBody::setVelocityMax(const glm::vec3 &newVelocityMax)
{
    m_velocityMax = newVelocityMax;
}

glm::vec3 RigidBody::getExpectedDisplacement() const
{
    return m_expectedDisplacement;
}

///////////////////////////////////////////////////////////////////////////////

RigidBody::RigidBody(Entity* entity, glm::vec3 vMax, BoxCollider* collider) :
    IGameComponent(entity),
    m_velocity(0),
    m_targetVelocityFactor(0),
    m_velocityMax(vMax),
    m_collider(collider)
{}

void RigidBody::applyGlobalForce(glm::vec3 force)
{

    // When there is no external force,
    // the factor is reset to 0 and the object is going to step
    // This helps simulate friction
    if(glm::length(force) == 0){
        return;
    }

    m_targetVelocityFactor += force;
    if(glm::length(m_targetVelocityFactor) == 0){
        return;
    }
}


bool RigidBody::isMovingInDirection(Direction dir)
{
    return m_velocity[dir] != 0;
}

bool RigidBody::isUsingGravity()
{
    return m_isGravity;
}

void RigidBody::update()
{
    if(!this->isEnabled()){
        return;
    }
    m_dt = MGlobalInstance::Get->deltaTime();
    m_expectedDisplacement = glm::vec3(0);

    applyGravityImpact();
    calculateVelocity();

    if(glm::length(m_velocity) == 0){
        return;
    }

    m_expectedDisplacement = m_velocity * m_dt;
    applyColliderImpact();
    updateRigidBodyMove();
    m_targetVelocityFactor = glm::vec3(0);
}


void RigidBody::updateRigidBodyMove()
{
    if(glm::length(m_expectedDisplacement) == 0){
        return;
    }
    this->getGameObject()->moveForwardGlobal(m_expectedDisplacement.z);
    this->getGameObject()->moveRightGlobal(m_expectedDisplacement.x);
    this->getGameObject()->moveUpGlobal(m_expectedDisplacement.y);
}

void RigidBody::calculateVelocity()
{
    /// The larger it is, the longer it take to change the velocity in the axis
    glm::vec3 dampFactor {0.1, 0.3, 0.1};
    m_velocity = glm::mix(m_velocity,
                          m_velocityMax * m_targetVelocityFactor,
                          m_dt / dampFactor);
}


void RigidBody::applyGravityImpact()
{
    if(!isGravity()){
        return;
    }
    m_targetVelocityFactor.y -= 1;
}


void RigidBody::applyColliderImpact()
{
    if(m_collider == nullptr || !m_collider->isEnabled()){
        return;
    }
    /// Map the move to global coordinate
    m_expectedDisplacement = m_collider->boxRayCast(m_expectedDisplacement);

    /// Update the final velocity based on the expected displacement
    m_velocity = m_expectedDisplacement / m_dt;
}
