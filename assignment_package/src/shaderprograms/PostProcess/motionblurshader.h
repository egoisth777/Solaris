#ifndef MOTIONBLURSHADER_H
#define MOTIONBLURSHADER_H

#include <shaderprograms/postprocessingshader.h>



class MotionBlurShader : public PostProcessingShader
{
public:
    int unifModel;
    int unifViewProjLastFrame;  // A handle for the "uniform" mat4 representing combined projection and view matrices last frame
    int unifVelocityMap;
public:
    MotionBlurShader(OpenGLContext* context);

    // Pass the given Projection * View matrix to this shader on the GPU
    void setModelMatrix(const glm::mat4 &model);
    void setViewProjMatrixLastFrame(const glm::mat4 &vp);
    void setUnifVelocityTex(int textureSlot);

    // ShaderProgram interface
public:
    void setupMemberVars() override;
    void draw(Drawable &d, int textureSlot = 0) override;
};

#endif // MOTIONBLURSHADER_H
