#include "camera.h"
#include "glm_includes.h"
#include "settings.h"


Camera::Camera(glm::vec3 pos)
    : Camera(400, 400, pos)
{}

Camera::Camera(unsigned int w, unsigned int h, glm::vec3 pos)
    : Entity(pos), m_fovy(45), m_width(w), m_height(h),
      m_near_clip(NEAR_CLIP), m_far_clip(FAR_CLIP), m_aspect(w / static_cast<float>(h))
{}

Camera::Camera(const Camera &c)
    : Entity(c),
      m_fovy(c.m_fovy),
      m_width(c.m_width),
      m_height(c.m_height),
      m_near_clip(c.m_near_clip),
      m_far_clip(c.m_far_clip),
      m_aspect(c.m_aspect)
{}


void Camera::setWidthHeight(unsigned int w, unsigned int h) {
    m_width = w;
    m_height = h;
    m_aspect = w / static_cast<float>(h);
}


void Camera::tick(float dT, InputBundle &input) {
    // Do nothing
}

glm::mat4 Camera::getViewProj() const {
    return glm::perspective(glm::radians(m_fovy), m_aspect, m_near_clip, m_far_clip) * glm::lookAt(m_position, m_position + m_forward, m_up);
}

glm::mat4 Camera::getView() const
{
    return glm::lookAt(m_position, m_position + m_forward, m_up);
}

glm::mat4 Camera::getProj() const
{
    return glm::perspective(glm::radians(m_fovy), m_aspect, m_near_clip, m_far_clip);
}

glm::vec3 Camera::getEye() const
{
    return m_position;
}

glm::ivec2 Camera::getDims() const
{
    return glm::ivec2(m_width, m_height);
}

glm::mat3 Camera::getRot() const
{
    return glm::mat3(m_right, m_up, m_forward);
}

glm::vec2 Camera::getNearFarClip() const
{
    return glm::vec2(m_near_clip, m_far_clip);
}


float Camera::getNearClip() const
{
    return m_near_clip;
}

float Camera::getFarClip() const
{
    return m_far_clip;
}

float Camera::getFovy() const
{
    return m_fovy;
}


float Camera::getAspect() const
{
    return m_aspect;
}
