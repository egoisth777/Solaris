#ifndef SKYSHADER_H
#define SKYSHADER_H

#include "postprocessingshader.h"
#include "scene/camera.h"

class SkyShader : public PostProcessingShader
{
public:
    int unifCameraPos;
    int unifView;
    int unifViewInv;
    int unifProj;

public:
    SkyShader(OpenGLContext* context);
    // Sets up shader-specific handles
    virtual void setupMemberVars() override;
    // Pass the given Projection * View matrix to this shader on the GPU
    void setCamera(const Camera* cam);
    // Draw the given object to our screen using this ShaderProgram's shaders
    //virtual void draw(Drawable &d, int textureSlot = 0) override;
};

#endif // SKYSHADER_H
