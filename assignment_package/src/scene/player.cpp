#include "player.h"
#include <QString>
#include <globalinstance.h>
#include <iostream>
#include <RigidBody/raycastor.h>
#include <RigidBody/rigidbody.h>


Player::Player(glm::vec3 pos, const Terrain &terrain)
    :   Entity(pos),
        m_velocity(0,0,0), m_acceleration(0),
        m_friction(0),
        m_camera(pos + glm::vec3(0, 1.5f, 0)),
        mcr_terrain(terrain),
        m_collider(this, mcr_terrain, glm::vec3(0.4f, .9f, 0.4f), glm::vec3(0, .9f, 0)),
        m_rigidBody(this, glm::vec3(10, 20, 10), &m_collider),
        mcr_camera(m_camera)
{}

Player::~Player()
{}


void Player::start(){
    /// On entity start, temporally set player to fly
    switchMovingMode(MovingState::FLY, true);
}

void Player::onGameStart()
{
    /// On game start, switch off flying mode
    switchMovingMode(MovingState::FLY, false);
    spawnPositionCheck();
}


void Player::tick(float, InputBundle &input) {
    m_rigidBody.update();
    processInputs(input);

    checkMovingResult();
    castLookRay();
}

void Player::spawnPositionCheck()
{
    for(int i = 512; i >= 0; --i){
        if(mcr_terrain.getBlockAt(mcr_position.x, i, mcr_position.z) != BlockType::EMPTY){
            m_position.y = i + 5;
            m_camera.setPosition(m_position + glm::vec3(0, 1.5, 0));
            return;
        }
    }
}

void Player::checkMovingResult()
{
    /// No need to check if under fly mode
    if(this->isUnderMovingState(MovingState::FLY)){
        return;
    }

    setFootOverlayBlock(this->mcr_terrain.getBlockAt(this->mcr_position));

    /// No need for camera checking if no callback is registered
    if(m_onCameraOverlayTypeChanged == nullptr || this->IsSigBlocked()){
        return;
    }

    setCameraOverlayBlock(this->mcr_terrain.getBlockAt(this->getCameraPos()));
}


std::optional<HitInfo> Player::getInteractivableCubeHit() const
{
    return m_interactivableCubeHit;
}

const glm::vec3 &Player::get_pos() const {return m_position;}

BoxCollider Player::getCollider() const
{
    return m_collider;
}

glm::vec3 Player::getCameraPos() const
{
    return m_camera.mcr_position;
}

void Player::processInputs(InputBundle &inputs) {
    // Update player moving state
    if(inputs.SwitchedChangeSpeedModifier){
        switchMovingMode(MovingState::SPEEDUP);
    }
    if(inputs.SwitchedChangeFlightMode){
        switchMovingMode(MovingState::FLY);
    }
    if(inputs.SwitchedChangePhotoMode){
        switchMovingMode(MovingState::PHOTO);
    }

    // Update the Player's velocity and acceleration based on the
    // state of the inputs.
    try{
        glm::vec2 mouseMouse = calcMouseSens(glm::vec2(inputs.mouseX, inputs.mouseY));
        rotateOnRightLocal(mouseMouse.y);
        rotateOnUpGlobal(mouseMouse.x);
    }
    catch (...){
        std::cerr << "Player Rotate Error..." << std::endl;
    }

    glm::vec3 movingForce(0);

    if(inputs.PressedJump){
        onJumpPressed();
    }

    if(inputs.PressedLeft){
        movingForce.x = -1;
    }
    else if(inputs.PressedRight){
        movingForce.x = 1;
    }

    if(inputs.PressedBackward){
        movingForce.z = -1;
    }
    else if(inputs.PressedForward){
        movingForce.z = 1;
    }
    if(isUnderMovingState(MovingState::FLY) || isUnderMovingState(MovingState::PHOTO)){
        if(inputs.PressedUp){
            movingForce.y = 1;
        }
        else if(inputs.PressedDown){
            movingForce.y = -1;
        }
    }

    updateJump();

    if(glm::length(movingForce) == 0){
        return;
    }

    m_rigidBody.applyGlobalForce(mapLocalAttribToGlobal(movingForce));
    return;
}

void Player::computePhysics(float, const Terrain&) {
    return;
}

void Player::setCameraWidthHeight(unsigned int w, unsigned int h) {
    m_camera.setWidthHeight(w, h);
}

glm::vec3 Player::mapLocalAttribToGlobal(glm::vec3 dir)
{
    if(isUnderMovingState(MovingState::FLY)){
        return glm::normalize(dir.z * m_forward + glm::vec3(0, dir.y, 0)  + dir.x * m_right);
    }
    else{
        return glm::normalize(dir.z * glm::normalize(m_forward * glm::vec3(1,0,1)) +
               dir.y * m_up +
                dir.x * glm::normalize(m_right * glm::vec3(1,0,1)));
    }
}


void Player::moveAlongLocalDir(glm::vec3 dir, float amount)
{
    if(!isUnderMovingState(MovingState::FLY)){
        dir.y = 0;
        glm::normalize(dir);
    }

    dir *= amount;
    moveAlongVector(dir);
}

void Player::moveAlongVector(glm::vec3 dir) {
    Entity::moveAlongVector(dir);
    m_camera.moveAlongVector(dir);
}
void Player::moveForwardLocal(float amount) {
    moveAlongVector(this->m_forward * amount);
}
void Player::moveRightLocal(float amount) {
    moveAlongVector(this->m_right * amount);
}

void Player::moveUpLocal(float amount) {
    Entity::moveUpLocal(amount);
    m_camera.moveUpLocal(amount);
}
void Player::moveForwardGlobal(float amount) {
    Entity::moveForwardGlobal(amount);
    m_camera.moveForwardGlobal(amount);
}
void Player::moveRightGlobal(float amount) {
    Entity::moveRightGlobal(amount);
    m_camera.moveRightGlobal(amount);
}
void Player::moveUpGlobal(float amount) {
    Entity::moveUpGlobal(amount);
    m_camera.moveUpGlobal(amount);
}
void Player::rotateOnForwardLocal(float degrees) {
    Entity::rotateOnForwardLocal(degrees);
    m_camera.rotateOnForwardLocal(degrees);
}
void Player::rotateOnRightLocal(float degrees) {
    Entity::rotateOnRightLocal(degrees);
    m_camera.rotateOnRightLocal(degrees);

    /// Prevent rotate up-side-down
    float upAngle = acos(glm::dot(m_up, glm::vec3(0,1,0)));
    if(glm::abs(glm::degrees(upAngle)) > 80.0f){
        rotateOnRightLocal(-degrees);
    }

}
void Player::rotateOnUpLocal(float degrees) {
    Entity::rotateOnUpLocal(degrees);
    m_camera.rotateOnUpLocal(degrees);
}
void Player::rotateOnForwardGlobal(float degrees) {
    Entity::rotateOnForwardGlobal(degrees);
    m_camera.rotateOnForwardGlobal(degrees);
}
void Player::rotateOnRightGlobal(float degrees) {
    Entity::rotateOnRightGlobal(degrees);
    m_camera.rotateOnRightGlobal(degrees);
}
void Player::rotateOnUpGlobal(float degrees) {
    Entity::rotateOnUpGlobal(degrees);
    m_camera.rotateOnUpGlobal(degrees);
}


QString Player::posAsQString() const {
    std::string str("( " + std::to_string(m_position.x) + ", " + std::to_string(m_position.y) + ", " + std::to_string(m_position.z) + ")");
    return QString::fromStdString(str);
}
QString Player::velAsQString() const {
    std::string str("( " + std::to_string(m_rigidBody.velocity().x) + ", " + std::to_string(m_rigidBody.velocity().y) + ", " + std::to_string(m_rigidBody.velocity().z) + ")");
    return QString::fromStdString(str);
}
QString Player::accAsQString() const {
    std::string str("( " + std::to_string(m_rigidBody.targetVelocityFactor().x) + ", " + std::to_string(m_rigidBody.targetVelocityFactor().y) + ", " + std::to_string(m_rigidBody.targetVelocityFactor().z) + ")");
    return QString::fromStdString(str);
}
QString Player::lookAsQString() const {
    std::string str("( " + std::to_string(m_forward.x) + ", " + std::to_string(m_forward.y) + ", " + std::to_string(m_forward.z) + ")");
    return QString::fromStdString(str);
}

void Player::onMovingStateChangeTo(MEntity::EntityMovingState state, bool val)
{
    //std::cout << (int)state << ' ' << val << std::endl;
    switch (state){
        case MovingState::SPEEDUP:
            m_speedModifier *= (val ? SPEED_UP_MODIFIER : 1.0f / SPEED_UP_MODIFIER);
            m_rigidBody.setVelocityMax(MAX_SPEED * m_speedModifier);
            break;

        case MovingState::SWIM:
            m_speedModifier *= (val ? SPEED_SWIM_MODIFIER : 1.0f / SPEED_SWIM_MODIFIER);
            m_rigidBody.setVelocityMax(MAX_SPEED * m_speedModifier);
            break;

        case MovingState::FLY:
            enterFlyMode(val);
            break;

        case MovingState::PHOTO:
            enterPhotoMode(val);
            break;

        default:
            break;
        }
}

void Player::setUnconstrainMove(bool target)
{
    m_rigidBody.setIsGravity(!target);
    m_collider.setEnable(!target);
    m_rigidBody.setVelocity(glm::vec3(0));
}

bool Player::isInGround()
{
    return m_collider.isCollidingOnDir(Direction::YNEG);
}

bool Player::isSwimming()
{
    return this->isUnderMovingState(MovingState::SWIM);
}

void Player::setCameraOverlayBlock(BlockType cameraOverlayBlock)
{
    if(m_cameraOverlayBlock == cameraOverlayBlock){
        return;
    }

    m_cameraOverlayBlock = cameraOverlayBlock;
    m_onCameraOverlayTypeChanged(m_cameraOverlayBlock);
}

void Player::setFootOverlayBlock(BlockType footOverlayBlock)
{
    if(m_footOverlayBlock == footOverlayBlock){
        return;
    }
    m_footOverlayBlock = footOverlayBlock;
    switch(footOverlayBlock){
        case BlockType::WATER:
        case BlockType::LAVA:
            switchMovingMode(MovingState::SWIM);
            break;
        default:
            switchMovingMode(MovingState::SWIM, false);
            break;
        }
}

glm::vec2 Player::calcMouseSens(glm::vec2 mouseMove) const
{
    return m_mosueSenseModifier * mouseMove;
}

void Player::onJumpPressed()
{
    // Moving upward if is swimming
    if(isSwimming()){
        m_rigidBody.applyGlobalForce(glm::vec3(0, JUMP_FORCE, 0));
        return;
    }

    if(!isInGround()){
        return;
    }

    // Remove Gravity Force to get instant upward
    m_rigidBody.setVelocity(m_rigidBody.velocity() * glm::vec3(1, 0, 1));
    jump_timer = JUMP_FORCE_TIME;
}

void Player::updateJump()
{
    if(jump_timer < 0){
        return;
    }

    jump_timer -= MGlobalInstance::Get->deltaTime();
    m_rigidBody.applyGlobalForce(glm::vec3(0, JUMP_FORCE, 0));
}

void Player::enterFlyMode(bool isEnter)
{
    if(isEnter){
        // Reset overlay block
        setFootOverlayBlock(BlockType::EMPTY);
        setCameraOverlayBlock(BlockType::EMPTY);
    }

    if(!isUnderMovingState(MovingState::PHOTO)){
        setUnconstrainMove(isEnter);
    }
}

void Player::enterPhotoMode(bool val)
{
    m_mosueSenseModifier    = val ? glm::vec2(MOUSE_PHOTO_MODIFIER, MOUSE_PHOTO_MODIFIER) : glm::vec2(1);
    m_speedModifier         *= (val ? SPEED_PHOTO_MODIFIER : 1.0f / SPEED_PHOTO_MODIFIER);
    m_rigidBody.setVelocityMax(MAX_SPEED * m_speedModifier);

    if(!isUnderMovingState(MovingState::FLY)){
        setUnconstrainMove(val);
    }

}

void Player::castLookRay()
{
    RayCastor::generateRay(mcr_camera.mcr_position, m_forward, INTERACTION_LENGTH);
    m_interactivableCubeHit = RayCastor::castRayToValidTarrain(mcr_terrain);
}

void Player::registerCameraOverlayTypeChangeAction(std::function<void (BlockType)> func)
{
    m_onCameraOverlayTypeChanged = func;
}

void Player::unRegisterCameraOverlayTypeChangeAction()
{
    m_onCameraOverlayTypeChanged = nullptr;
}
