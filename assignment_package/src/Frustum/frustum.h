#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "scene/plane.h"
#include <Interface/IUpdateComponent.h>

#include <scene/camera.h>
#include <array>

#include "settings.h"


class Frustum : IUpdateComponent
{
public:
    enum FrustumFaceEnum : int{
        NearFace    = 0,
        FarFace     = 1,
        LeftFace    = 2,
        RightFace   = 3,
        TopFace     = 4,
        BottomFace  = 5
    };

    float CSM_Partitions[4];

private:
    const Camera* m_camera;
    std::array<Plane, 6> m_planes;

public:
    Frustum(const Camera*);

    /// Check if a point represented by vec3 is inside the frustum
    bool isOnFrustum(const glm::vec3&) const;

    /// Given the corner of a bounding box and its xyz extends,
    /// check if the box overlays with the frustum
    bool isOnFrustumBoxCorner(const glm::vec3&, const glm::vec3&) const;

private:
    bool isOnOrFrontPlaneBoxCorner(const Plane&,
                                   const glm::vec3&,
                                   const glm::vec3&) const;



    // IUpdateComponent interface
public:
    //void awake() override;

    /// Update the frustum planes from the camera
    void update() override;
    std::array<glm::mat4, 3> getCascadedLightViewProj();

    //void setEnable(bool enable) override;

};

#endif // FRUSTUM_H
