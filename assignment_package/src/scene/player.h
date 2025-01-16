#pragma once
#include "entity.h"
#include "camera.h"
#include "terrain.h"

#include <RigidBody/boxcollider.h>
#include <RigidBody/raycastor.h>
#include <RigidBody/rigidbody.h>

class Player : public Entity, public ISignalEmitter {

#define JUMP_FORCE              1.75f
#define JUMP_FORCE_TIME         0.2f
#define MAX_SPEED               glm::vec3(10, 20, 10)
#define SPEED_UP_MODIFIER       1.5f
#define SPEED_PHOTO_MODIFIER    0.2f
#define MOUSE_PHOTO_MODIFIER    0.1f
#define SPEED_SWIM_MODIFIER     glm::vec3(0.5, 0.3, 0.5)
#define INTERACTION_LENGTH      3.0f

private:
    glm::vec3 m_velocity, m_acceleration;
    glm::vec3 m_friction;
    Camera m_camera;
    const Terrain &mcr_terrain;

    std::optional<HitInfo> m_interactivableCubeHit;

    void processInputs(InputBundle &inputs);
    void computePhysics(float dT, const Terrain &terrain);

    BlockType m_footOverlayBlock    =   BlockType::EMPTY;
    BlockType m_cameraOverlayBlock  =   BlockType::EMPTY;

    // Physics
private:
    BoxCollider m_collider;
    RigidBody   m_rigidBody;
    float       jump_timer = 0;
    glm::vec3   m_speedModifier = glm::vec3(1);
    glm::vec2   m_mosueSenseModifier = glm::vec2(1);


public:
    // Readonly public reference to our camera
    // for easy access from MyGL
    const Camera& mcr_camera;

    Player(glm::vec3 pos, const Terrain &terrain);
    virtual ~Player() override;

    void setCameraWidthHeight(unsigned int w, unsigned int h);

    void start();
    void onGameStart();
    void tick(float dT, InputBundle &input) override;


    /// Call in start()
    /// Check the position of the player and put it properly based on the terrain
    void spawnPositionCheck();

    void checkMovingResult();

    // Map a local displacement to global displacement, consider flight mode
    glm::vec3 mapLocalAttribToGlobal(glm::vec3 dir) override;

    // Move function with y component discard in non-flight mode
    void moveAlongLocalDir(glm::vec3 dir, float amount);

    // Player overrides all of Entity's movement
    // functions so that it transforms its camera
    // by the same amount as it transforms itself.
    void moveAlongVector(glm::vec3 dir) override;
    void moveForwardLocal(float amount) override;
    void moveRightLocal(float amount) override;
    void moveUpLocal(float amount) override;
    void moveForwardGlobal(float amount) override;
    void moveRightGlobal(float amount) override;
    void moveUpGlobal(float amount) override;
    void rotateOnForwardLocal(float degrees) override;
    void rotateOnRightLocal(float degrees) override;
    void rotateOnUpLocal(float degrees) override;
    void rotateOnForwardGlobal(float degrees) override;
    void rotateOnRightGlobal(float degrees) override;
    void rotateOnUpGlobal(float degrees) override;

    // For sending the Player's data to the GUI
    // for display
    QString posAsQString() const;
    QString velAsQString() const;
    QString accAsQString() const;
    QString lookAsQString() const;

    // Entity interface
    std::optional<HitInfo> getInteractivableCubeHit() const;
    const glm::vec3& get_pos() const;

    BoxCollider getCollider() const;
    glm::vec3 getCameraPos() const;
    bool isInGround();
    bool isSwimming();

    void setCameraOverlayBlock(BlockType);
    void setFootOverlayBlock(BlockType);

    glm::vec2 calcMouseSens(glm::vec2 mouseMove) const;

protected:
    void onMovingStateChangeTo(MEntity::EntityMovingState, bool) override;

    /// Make player move without the constraint of gravity and collider
    void setUnconstrainMove(bool target);

    // JUMP
private:
    /// Do jump or swim based on my move state
    void onJumpPressed();
    void updateJump();

    // FLY
private:
    void enterFlyMode(bool);

    // PHOTO mode
private:
    void enterPhotoMode(bool);

    // Ray Cast
private:

    /// Cast a ray based on camera's forward direction
    /// Set up the interactivable cube position
    void castLookRay();

    // Call back
private:
    std::function<void(BlockType)> m_onCameraOverlayTypeChanged = nullptr;
public:
    void registerCameraOverlayTypeChangeAction(std::function<void(BlockType)>);
    void unRegisterCameraOverlayTypeChangeAction();
};
