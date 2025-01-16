#ifndef RIGIDBODY_H
#define RIGIDBODY_H
#include "boxcollider.h"

#include <Interface/igamecomponent.h>
#include <scene/entity.h>

class RigidBody : public IGameComponent
{

private:
    glm::vec3 m_velocity;

    /// The target velocity the rigidbody want to reach
    /// Represent the scale of max velocity
    /// E.g. (2,2,2) means the rigidbody will finally reach twice the max_velocity
    glm::vec3 m_targetVelocityFactor;
    glm::vec3 m_velocityMax;

    /// The expected global displacement, computed from localDisplacement
    /// Calculated based on proper moving logic of the entity
    /// e.g. considering colliding, and cull the y component in forward and right axis
    glm::vec3 m_expectedDisplacement = glm::vec3(0);
    bool m_isGravity = true;

    Direction m_gravityDirection = Direction::YNEG;
    BoxCollider* m_collider;

    float m_dt;

    // IUpdateComponent interface
public:
    //void awake() override;
    //void start() override;

    /// If the rigidbody is disabled, no movement happens
    void update() override;

public:
    RigidBody(Entity*, glm::vec3 vMax = glm::vec3(10, 20, 10), BoxCollider* = nullptr);

    /// Cast a force to this rigidbody in global coord
    void applyGlobalForce(glm::vec3);

// Getter & Setter
public:
    bool isMovingInDirection(Direction);
    bool isUsingGravity();
    glm::vec3 velocity() const;
    void setVelocity(const glm::vec3 &newVelocity);
    void setTargetVelocityFactor(const glm::vec3 &newAcceleration);
    bool isGravity() const;
    void setIsGravity(bool newIsGravity);
    glm::vec3 velocityMax() const;
    void setVelocityMax(const glm::vec3 &newVelocityMax);

    glm::vec3 getExpectedDisplacement() const;

    glm::vec3 targetVelocityFactor() const;

private:
    void updateRigidBodyMove();

    /// Calculate the velocity based on the net force to the rigidobjct
    void calculateVelocity();

    /// The gravity is defaultly the max speed the rigidbody can move along gravity direction
    void applyGravityImpact();
    void applyColliderImpact();
};

#endif // RIGIDBODY_H
