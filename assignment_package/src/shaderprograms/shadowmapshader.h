#ifndef SHADOWMAPSHADER_H
#define SHADOWMAPSHADER_H

#include "shaderprograms/shaderprogram.h"
#include <array>
#include "globalinstance.h"

class ShadowMapShader : public ShaderProgram
{
private:

    int attrPos;
    int attrNor;
    int attrUV;

    int unifPosMap;
    int unifDepthBound;
    int unifModel;
    int unifView;
    int unifViewInv;
    int unifProj;
    int unifCameraPos;
    int unifViewProj;
    int unifLightViewProj;
    int unifLightDir;

    int unifLightDepthMap;

public:
    ShadowMapShader(OpenGLContext* context);
    virtual void setupMemberVars() override;
    void setModelMatrix(const glm::mat4 &model);
    void setViewProjMatrix(const glm::mat4 &vp);
    void setLight(std::array<glm::mat4, 3>& mats, Frustum* frustum);
    void setCamera(const Camera* cam);
    virtual void draw(Drawable &d, int textureSlot = 0) override;
    void drawFlat(Drawable &d, int textureSlot = 0);

};

#endif // SHADOWMAPSHADER_H
