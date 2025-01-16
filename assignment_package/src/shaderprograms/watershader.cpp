#include "shaderprograms/watershader.h"

WaterShader::WaterShader(OpenGLContext* context)
    : ShaderProgram(context),
      attrPos(-1), attrNor(-1),
      unifModel(-1), unifView(-1), unifViewInv(-1),
      unifProj(-1), unifViewProj(-1),
      unifCameraPos(-1), unifLightDir(-1),
    unifAlbedoMap(-1), unifPosMap(-1), unifSkyMap(-1)
{}

void WaterShader::setupMemberVars()
{
    attrPos  = context->glGetAttribLocation(prog, "vs_Pos");
    attrNor  = context->glGetAttribLocation(prog, "vs_Nor");

    unifModel      = context->glGetUniformLocation(prog, "u_Model");
    unifView       = context->glGetUniformLocation(prog, "u_View");
    unifViewInv    = context->glGetUniformLocation(prog, "u_ViewInv");
    unifProj       = context->glGetUniformLocation(prog, "u_Proj");
    unifViewProj   = context->glGetUniformLocation(prog, "u_ViewProj");
    unifCameraPos  = context->glGetUniformLocation(prog, "u_CameraPos");
    unifLightDir   = context->glGetUniformLocation(prog, "u_LightDir");

    unifAlbedoMap  = context->glGetUniformLocation(prog, "u_AlbedoMap");
    unifPosMap     = context->glGetUniformLocation(prog, "u_PosMap");
    unifSkyMap     = context->glGetUniformLocation(prog, "u_SkyMap");
    unifTime       = context->glGetUniformLocation(prog, "u_Time");
}

void WaterShader::setModelMatrix(const glm::mat4 &model)
{
    useMe();

    if (unifModel != -1) {
        // Pass a 4x4 matrix into a uniform variable in our shader
        // Handle to the matrix variable on the GPU
        context->glUniformMatrix4fv(unifModel,
                                    // How many matrices to pass
                                    1,
                                    // Transpose the matrix? OpenGL uses column-major, so no.
                                    GL_FALSE,
                                    // Pointer to the first element of the matrix
                                    &model[0][0]);
    }
}

void WaterShader::setViewProjMatrix(const glm::mat4 &vp)
{
    useMe();

    if(unifViewProj != -1) {
        // Pass a 4x4 matrix into a uniform variable in our shader
        // Handle to the matrix variable on the GPU
        context->glUniformMatrix4fv(unifViewProj,
                                    // How many matrices to pass
                                    1,
                                    // Transpose the matrix? OpenGL uses column-major, so no.
                                    GL_FALSE,
                                    // Pointer to the first element of the matrix
                                    &vp[0][0]);
    }
}

void WaterShader::setLightDir()
{
    useMe();

    if(unifLightDir != -1) {
        glm::vec3 dir = MGlobalInstance::Get->getLightAngle();
        context->glUniform3fv(unifLightDir, 1, &dir[0]);
    }
}

void WaterShader::setCamera(const Camera* cam)
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

void WaterShader::draw(Drawable &d, int textureSlot)
{
    if (!d.initialized()) return;

    useMe();
    ++DrawCalls;

    if(d.elemCount() < 0) {
        throw std::out_of_range("Attempting to draw a drawable with m_count of " + std::to_string(d.elemCount()) + "!");
    }

    if(unifAlbedoMap != -1)
    {
        context->glUniform1i(unifAlbedoMap, textureSlot);
    }

    if (unifPosMap != -1)
    {
        context->glUniform1i(unifPosMap, textureSlot + 1);
    }

    if (unifSkyMap != -1)
    {
        context->glUniform1i(unifSkyMap, textureSlot + 2);
    }

    if (d.bindVert())
    {
        if (attrPos != -1)
        {
            context->glEnableVertexAttribArray(attrPos);
            context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, sizeof(Drawable::VertexData), (void*)0);
        }

        if (attrNor != -1)
        {
            context->glEnableVertexAttribArray(attrNor);
            context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, sizeof(Drawable::VertexData), (void*)(sizeof(float)*4));
        }
    }

    d.bindIdx();
    context->glDrawElements(d.drawMode(), d.elemCount(), GL_UNSIGNED_INT, 0);

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);
    context->printGLErrorLog();
}

void WaterShader::drawTransp(Drawable &d, int textureSlot)
{
    if (d.traElemCount() < 1 || !d.ready) return;

    useMe();
    ++DrawCalls;

    if(d.traElemCount() < 0) {
        throw std::out_of_range("Attempting to draw a drawable with m_count of " + std::to_string(d.elemCount()) + "!");
    }

    if(unifAlbedoMap != -1)
    {
        context->glUniform1i(unifAlbedoMap, textureSlot);
    }

    if (unifPosMap != -1)
    {
        context->glUniform1i(unifPosMap, textureSlot + 1);
    }

    if (unifSkyMap != -1)
    {
        context->glUniform1i(unifSkyMap, textureSlot + 2);
    }

    if (d.bindTransp())
    {
        if (attrPos != -1)
        {
            context->glEnableVertexAttribArray(attrPos);
            context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, sizeof(Drawable::VertexData), (void*)0);
        }

        if (attrNor != -1)
        {
            context->glEnableVertexAttribArray(attrNor);
            context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, sizeof(Drawable::VertexData), (void*)(sizeof(float)*4));
        }
    }

    d.bindTraIdx();
    context->glDrawElements(d.drawMode(), d.traElemCount(), GL_UNSIGNED_INT, 0);

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);
    context->printGLErrorLog();
}
