#pragma once

#include "Enums/EntityMovingStateEnums.h"
#include "Interface/igamecomponent.h"
#include "glm_includes.h"
#include "smartpointerhelp.h"
#include <vector>

struct InputBundle {
    bool PressedForward, PressedBackward,
        PressedLeft, PressedRight,
        PressedUp, PressedDown;
    bool PressedJump;
    float mouseX, mouseY;

    bool SwitchedChangeFlightMode = false;
    bool SwitchedChangePhotoMode = false;
    bool SwitchedChangeSpeedModifier = false;

    // Shader profiler
    bool EnableShaderFeature1 = false;
    bool EnableShaderFeature2 = false;
    bool EnableShaderFeature3 = false;

    bool RenderTimeSpeedUp    = false;
    bool RenderTimeSpeedDown  = false;
    bool RenderTimeSpeedReverse = false;


    InputBundle():
        PressedForward(false),
        PressedBackward(false),
        PressedLeft(false),
        PressedRight(false),
        PressedUp(false),
        PressedDown(false),
        PressedJump(false),
        mouseX(0.f), mouseY(0.f),
        SwitchedChangeFlightMode(false),
        SwitchedChangeSpeedModifier(false)
    {}
};

class Entity {
protected:
    // Vectors that define the axes of our local coordinate system
    glm::vec3 m_forward, m_right, m_up;
    // The origin of our local coordinate system
    glm::vec3 m_position;
    MovingState m_movingState = MovingState::WALKING;

    // Components, call in every tick
    std::vector<uPtr<IGameComponent>> m_components;
    std::vector<uPtr<IGameComponent>>& getComponents();

public:
    // A readonly reference to position for external use
    const glm::vec3& mcr_position;

    // Various constructors
    Entity();
    Entity(glm::vec3 pos);
    Entity(const Entity &e);

    MovingState getMovingState() const;
    bool isUnderMovingState(MovingState) const;
    void setPosition(glm::vec3);

    // Virtuals
public:
    virtual ~Entity();

    // To be called by MyGL::tick()
    virtual void tick(float dT, InputBundle &input) = 0;

    // Map a local direction to global coord, normalized
    virtual glm::vec3 mapLocalAttribToGlobal(glm::vec3 dir);

    // Translate along the given vector
    virtual void moveAlongVector(glm::vec3 dir);

    // Translate along one of our local axes
    virtual void moveForwardLocal(float amount);
    virtual void moveRightLocal(float amount);
    virtual void moveUpLocal(float amount);

    // Translate along one of the world axes
    virtual void moveForwardGlobal(float amount);
    virtual void moveRightGlobal(float amount);
    virtual void moveUpGlobal(float amount);

    // Rotate about one of our local axes
    virtual void rotateOnForwardLocal(float degrees);
    virtual void rotateOnRightLocal(float degrees);
    virtual void rotateOnUpLocal(float degrees);

    // Rotate about one of the world axes
    virtual void rotateOnForwardGlobal(float degrees);
    virtual void rotateOnRightGlobal(float degrees);
    virtual void rotateOnUpGlobal(float degrees);
    void switchMovingMode(MovingState);
    void switchMovingMode(MovingState, bool toTarget);

    glm::vec3 getForward() const;
    glm::vec3 getRight() const;
    glm::vec3 getUp() const;

protected:
    MovingState& getMovingState();
    virtual void onMovingStateChangeTo(MovingState, bool);
};
