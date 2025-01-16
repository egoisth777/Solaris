#ifndef WATERSHADER_H
#define WATERSHADER_H

#include "shaderprograms/shaderprogram.h"
#include "globalinstance.h"

class WaterShader : public ShaderProgram
{
private:
    int attrPos;
    int attrNor;

    int unifModel;
    int unifView;
    int unifViewInv;
    int unifProj;
    int unifViewProj;
    int unifCameraPos;
    int unifLightDir;

    int unifAlbedoMap;
    int unifPosMap;
    int unifSkyMap;

public:
    WaterShader(OpenGLContext* context);
    virtual void setupMemberVars() override;
    void setModelMatrix(const glm::mat4 &model);
    void setViewProjMatrix(const glm::mat4 &vp);
    void setLightDir();
    void setCamera(const Camera* cam);
    virtual void draw(Drawable &d, int textureSlot = 0) override;
    virtual void drawTransp(Drawable &d, int textureSlot = 0) override;

};

#endif // WATERSHADER_H
