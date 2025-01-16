#include "skyshader.h"

SkyShader::SkyShader(OpenGLContext* context)
    : PostProcessingShader(context), unifCameraPos(-1),
      unifView(-1), unifViewInv(-1), unifProj(-1)
{}

void SkyShader::setupMemberVars()
{
    PostProcessingShader::setupMemberVars();

    unifCameraPos   = context->glGetUniformLocation(prog, "u_CameraPos");
    unifView        = context->glGetUniformLocation(prog, "u_View");
    unifViewInv     = context->glGetUniformLocation(prog, "u_ViewInv");
    unifProj        = context->glGetUniformLocation(prog, "u_Proj");
}

void SkyShader::setCamera(const Camera* cam)
{
    useMe();

    if (unifView != -1)
    {
        glm::mat4 view = cam->getView();
        context->glUniformMatrix4fv(unifView,
                                    1,
                                    GL_FALSE,
                                    &view[0][0]);
    }

    if (unifViewInv != -1)
    {
        glm::mat3 viewInv = cam->getRot();
        context->glUniformMatrix3fv(unifViewInv,
                                    1,
                                    GL_FALSE,
                                    &viewInv[0][0]);
    }

    if (unifProj != -1)
    {
        glm::mat4 proj = cam->getProj();
        context->glUniformMatrix4fv(unifProj,
                                    1,
                                    GL_FALSE,
                                    &proj[0][0]);
    }

    if (unifCameraPos != -1)
    {
        glm::vec3 pos = cam->getEye();
        context->glUniform3f(unifCameraPos,pos.x,pos.y,pos.z);
    }
}
