#include "frustum.h"
#include "globalinstance.h"

Frustum::Frustum(const Camera * cam) :
    m_camera(cam)
{
    m_planes.fill(Plane());

    CSM_Partitions[0] = m_camera->getNearClip();
    float ratio = m_camera->getFarClip() / m_camera->getNearClip();
    float range = m_camera->getFarClip() - m_camera->getNearClip();
    for (int i = 1; i < 4; ++i)
    {
        float log = m_camera->getNearClip() * std::pow(ratio, i / 3.f);
        float uniform = m_camera->getNearClip() + range * i / 3.f;
        CSM_Partitions[i] = 0.6 * log + 0.4 * uniform;
    }
}

bool Frustum::isOnFrustum(const glm::vec3 &pos) const
{
    return isOnFrustumBoxCorner(pos, glm::vec3(0.1f));
}


bool Frustum::isOnFrustumBoxCorner(const glm::vec3 &corner, const glm::vec3 &extend) const
{
    for(auto& plane : m_planes){
        if(!isOnOrFrontPlaneBoxCorner(plane, corner, extend)){
            return false;
        }
    }
    return true;
}


bool Frustum::isOnOrFrontPlaneBoxCorner(const Plane &plane,
                                        const glm::vec3 &corner,
                                        const glm::vec3 &extend) const
{
    glm::vec3 furtherestPoint;
    glm::vec3 closestPoint;

    for(int i = 0; i < 3; ++i){
        if(plane.normal[i] > 0){
            furtherestPoint[i] = corner[i] + extend[i];
            closestPoint[i] = corner[i];
        }
        else{
            furtherestPoint[i] = corner[i];
            closestPoint[i] = corner[i] + extend[i];
        }
    }

    float farDist = plane.getSignedDistanceToPlane(furtherestPoint);
    float nearDist = plane.getSignedDistanceToPlane(closestPoint);

    if (farDist <=  0 && nearDist <= 0)
    {
        return false;
    }
    return true;
}



void Frustum::update()
{
    const float halfVertical = m_camera->getFarClip() * tanf(m_camera->getFovy() * .5f);
    const float halfHorizontal = halfVertical * m_camera->getAspect();
    const glm::vec3 farFront = m_camera->getFarClip() * m_camera->getForward();
    const glm::vec3 cameraPos = m_camera->mcr_position;

    this->m_planes[NearFace].normal     = m_camera->getForward();
    this->m_planes[FarFace].normal      = -m_camera->getForward();

    this->m_planes[NearFace].pointOnPlane   = cameraPos + m_camera->getNearClip() * m_camera->getForward();
    this->m_planes[FarFace].pointOnPlane    = cameraPos + m_camera->getFarClip() * m_camera->getForward();


    this->m_planes[RightFace].normal    = glm::normalize(glm::cross(m_camera->getUp(),
                                                                farFront + m_camera->getRight() * halfHorizontal));
    this->m_planes[LeftFace].normal     = glm::normalize(glm::cross(farFront - m_camera->getRight() * halfHorizontal,
                                                                 m_camera->getUp()));
    this->m_planes[RightFace].pointOnPlane  = cameraPos;
    this->m_planes[LeftFace].pointOnPlane   = cameraPos;


    this->m_planes[TopFace].normal      = glm::cross(farFront + m_camera->getUp() * halfVertical,
                                                m_camera->getRight());
    this->m_planes[BottomFace].normal   = glm::cross(m_camera->getRight(),
                                                farFront - m_camera->getUp() * halfVertical);


    this->m_planes[TopFace].pointOnPlane    = cameraPos;
    this->m_planes[BottomFace].pointOnPlane = cameraPos;
}


std::array<glm::mat4, 3> Frustum::getCascadedLightViewProj()
{
    float farClip = m_camera->getFarClip();
    float nearClip = m_camera->getNearClip();
    glm::vec4 camPos = glm::vec4(m_camera->getEye(), 1);

    // TODO: cache this
    const float halfVertical = m_camera->getFarClip() * tanf(m_camera->getFovy() * .5f);
    const float halfHorizontal = halfVertical * m_camera->getAspect();

    const glm::vec3 lightDir = MGlobalInstance::Get->getLightAngle();
    const glm::mat4 invViewProj = glm::inverse(m_camera->getViewProj());

    // in camera NDC space
    std::array<glm::vec4, 4> farPlane
    {
        glm::vec4(-1, -1, 1, 1),
        glm::vec4(1, -1, 1, 1),
        glm::vec4(1, 1, 1, 1),
        glm::vec4(-1, 1, 1, 1)
    };
    // to world space
    for (auto& v : farPlane)
    {
        v = invViewProj * v;
        v /= v.w;
    }

    std::array<glm::vec4, 8> frustumCorners;

    std::array<glm::mat4, 3> lightViewProjMats;

    for (int i = 0; i < 3; ++i)
    {
        float nearRatio = CSM_Partitions[i] / farClip;
        float farRatio = CSM_Partitions[i + 1] / farClip;
        glm::vec4 nearCenter = glm::vec4(0);
        glm::vec4 farCenter = glm::vec4(0);

        for (int j = 0; j < 4; ++j)
        {
            frustumCorners[j] = nearRatio * farPlane[j] + (1.f - nearRatio) * camPos;
            nearCenter += frustumCorners[j];
        }
        nearCenter = nearCenter / 4.f;

        for (int j = 0; j < 4; ++j)
        {
            frustumCorners[j + 4] = farRatio * farPlane[j] + (1.f - farRatio) * camPos;
            farCenter += frustumCorners[j + 4];
        }
        farCenter = farCenter / 4.f;

        /*
        glm::vec4 center = glm::vec4(0);
        for (const auto& v : frustumCorners)
        {
            center += v;
        }
        center = center / 8.f;
        float radius = glm::length(frustumCorners[6] - frustumCorners[0]) / 2.f;
        glm::vec3 up = glm::cross(-lightDir, glm::vec3(1, 0, 0));
        glm::mat4 lightView = glm::lookAt(glm::vec3(center) + lightDir, glm::vec3(center), up);

        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::lowest();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::lowest();
        for (const auto& v : frustumCorners)
        {
            const auto trf = lightView * v;
            minX = std::min(minX, trf.x);
            maxX = std::max(maxX, trf.x);
            minY = std::min(minY, trf.y);
            maxY = std::max(maxY, trf.y);
            minZ = std::min(minZ, trf.z);
            maxZ = std::max(maxZ, trf.z);
        }

        constexpr float zMult = 4.0f;
        if (minZ < 0)
        {
            minZ *= zMult;
        }
        else
        {
            minZ /= zMult;
        }
        if (maxZ < 0)
        {
            maxZ /= zMult;
        }
        else
        {
            maxZ *= zMult;
        }
        glm::mat4 lightProj = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
        lightViewProjMats[i] = lightProj * lightView;
*/


        glm::vec3 tmp;
        float c2 = glm::length2(frustumCorners[7] - farCenter);
        float d2 = glm::length2(frustumCorners[3] - nearCenter);
        float e2 = glm::length2(farCenter - nearCenter);
        float alpha = (c2 < (d2 + e2)) ? 0.5f - (c2 - d2) / (2.f * e2) : 0.f;
        glm::vec4 center = alpha * nearCenter + (1.f - alpha) * farCenter;
        float radius = glm::length(frustumCorners[7] - center);

        float texelPerUnit = SHADOW_MAP_RES / (radius * 2.f);
        //glm::vec3 up = glm::cross(-lightDir, glm::vec3(1, 0, 0));
        glm::vec3 up = glm::vec3(0, 1, 0);

        glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(texelPerUnit, texelPerUnit, texelPerUnit));
        glm::mat4 lookAt = glm::lookAt(glm::vec3(0), -lightDir, up);
        lookAt = scale * lookAt;

        center = lookAt * center;
        center.x = glm::floor(center.x);
        center.y = glm::floor(center.y);
        center = glm::inverse(lookAt) * center;

        glm::mat4 lightView = glm::lookAt(glm::vec3(center) + radius * lightDir, glm::vec3(center), up);
        glm::mat4 lightProj = glm::ortho(-radius, radius, -radius, radius, 0.f, 2.f * radius);
        lightViewProjMats[i] = lightProj * lightView;

    }

    return lightViewProjMats;
}
