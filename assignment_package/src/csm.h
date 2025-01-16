#ifndef CSM_H
#define CSM_H

#include <glm_includes.h>
#include <openglcontext.h>

#include "shaderprograms/SurfaceShader.h"
#include "shaderprograms/shadowmapshader.h"
#include "settings.h"

class CascadedShadowMap
{
public:
    SurfaceShader progLightDepth;
    ShadowMapShader progShadowMap;

private:
    const int LayerNumber = 3;
    const int CSM_RES = SHADOW_MAP_RES;
    GLuint lightFBO;
    GLuint lightDepthMaps;

    GLuint shadowFBO;
    GLuint shadowMap;
    GLuint shadowDepth;


public:
    CascadedShadowMap(OpenGLContext* context);

    void initialize();

    void bindLightDepthBuffer(int layer);

    void bindLightDepthTexture(int slot);

    void bindShadowFBO();

    void bindShadowMap(int slot);


protected:
    OpenGLContext* mp_context;
};

#endif // CSM_H
