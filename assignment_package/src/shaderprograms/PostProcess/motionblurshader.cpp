#include "motionblurshader.h"


MotionBlurShader::MotionBlurShader(OpenGLContext *context):
    PostProcessingShader(context)
{}


void MotionBlurShader::setModelMatrix(const glm::mat4 &model)
{
    useMe();

    if(unifModel != -1)
    {
        context->glUniformMatrix4fv(unifModel,
                                    // How many matrices to pass
                                    1,
                                    // Transpose the matrix? OpenGL uses column-major, so no.
                                    GL_FALSE,
                                    // Pointer to the first element of the matrix
                                    &model[0][0]);
    }
}

void MotionBlurShader::setViewProjMatrixLastFrame(const glm::mat4 &vp)
{
    useMe();

    if(unifViewProjLastFrame != -1)
    {
        context->glUniformMatrix4fv(unifViewProjLastFrame,
                                    // How many matrices to pass
                                    1,
                                    // Transpose the matrix? OpenGL uses column-major, so no.
                                    GL_FALSE,
                                    // Pointer to the first element of the matrix
                                    &vp[0][0]);
    }
}

void MotionBlurShader::setupMemberVars()
{
    PostProcessingShader::setupMemberVars();
    unifViewProjLastFrame   = context->glGetUniformLocation(prog, "u_ViewProjLastFrame");
    unifModel   = context->glGetUniformLocation(prog, "u_Model");
    unifVelocityMap   = context->glGetUniformLocation(prog, "u_VelocityMap");
}

void MotionBlurShader::draw(Drawable &d, int textureSlot)
{
    PostProcessingShader::draw(d, textureSlot);
}

void MotionBlurShader::setUnifVelocityTex(int textureSlot)
{
    useMe();
    if(unifVelocityMap == -1){
        return;
    }
    context->glUniform1i(unifVelocityMap, textureSlot);
}

