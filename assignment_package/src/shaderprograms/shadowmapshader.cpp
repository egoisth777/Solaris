#include "shaderprograms/shadowmapshader.h"

ShadowMapShader::ShadowMapShader(OpenGLContext* context)
    : ShaderProgram(context),
      attrPos(-1), attrNor(-1), attrUV(-1),
      unifPosMap(-1), unifDepthBound(-1),
      unifModel(-1), unifViewProj(-1),
      unifView(-1), unifViewInv(-1), unifProj(-1), unifCameraPos(-1),
      unifLightViewProj(-1), unifLightDir(-1),
      unifLightDepthMap(-1)
{}

void
ShadowMapShader::setupMemberVars()
{
    attrPos  = context->glGetAttribLocation(prog, "vs_Pos");
    attrNor  = context->glGetAttribLocation(prog, "vs_Nor");
    attrUV  = context->glGetAttribLocation(prog, "vs_UV");

    unifDepthBound = context->glGetUniformLocation(prog, "u_DepthBound");
    unifModel      = context->glGetUniformLocation(prog, "u_Model");
    unifViewProj   = context->glGetUniformLocation(prog, "u_ViewProj");
    unifView       = context->glGetUniformLocation(prog, "u_View");
    unifViewInv    = context->glGetUniformLocation(prog, "u_ViewInv");
    unifProj       = context->glGetUniformLocation(prog, "u_Proj");
    unifCameraPos  = context->glGetUniformLocation(prog, "u_CameraPos");
    unifLightViewProj = context->glGetUniformLocation(prog, "u_LightViewProj");
    unifLightDir   = context->glGetUniformLocation(prog, "u_LightDir");
    unifTime       = context->glGetUniformLocation(prog, "u_Time");

    unifLightDepthMap = context->glGetUniformLocation(prog, "u_LightDepthMap");
    unifPosMap     = context->glGetUniformLocation(prog, "u_PosMap");
}

void ShadowMapShader::setModelMatrix(const glm::mat4 &model)
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

void ShadowMapShader::setViewProjMatrix(const glm::mat4 &vp)
{
    // Tell OpenGL to use this shader program for subsequent function calls
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

void ShadowMapShader::setLight(std::array<glm::mat4, 3>& mats, Frustum* frustum)
{
    useMe();

    if(unifLightViewProj != -1) {
        // Pass a 4x4 matrix into a uniform variable in our shader
        // Handle to the matrix variable on the GPU
        context->glUniformMatrix4fv(unifLightViewProj,
                                    // How many matrices to pass
                                    3,
                                    // Transpose the matrix? OpenGL uses column-major, so no.
                                    GL_FALSE,
                                    // Pointer to the first element of the matrix
                                    glm::value_ptr(mats[0]));
    }

    if(unifDepthBound != -1) {
        context->glUniform1fv(unifDepthBound, 4, frustum->CSM_Partitions);
    }

    if(unifLightDir != -1) {
        glm::vec3 dir = MGlobalInstance::Get->getLightAngle();
        context->glUniform3fv(unifLightDir, 1, &dir[0]);
    }
}

void ShadowMapShader::setCamera(const Camera* cam)
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


void ShadowMapShader::draw(Drawable &d, int textureSlot)
{
    if (!d.initialized()) return;

    useMe();
    ++DrawCalls;

    if(d.elemCount() < 0) {
        throw std::out_of_range("Attempting to draw a drawable with m_count of " + std::to_string(d.elemCount()) + "!");
    }

    if(unifLightDepthMap != -1)
    {
        context->glUniform1i(unifLightDepthMap, textureSlot);
    }

    if (unifPosMap != -1)
    {
        context->glUniform1i(unifPosMap, textureSlot + 1);
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

        if (attrUV != -1)
        {
            context->glEnableVertexAttribArray(attrUV);
            context->glVertexAttribPointer(attrUV, 2, GL_FLOAT, false, sizeof(Drawable::VertexData), (void*)(sizeof(float)*8));
        }
    }

    // Bind the index buffer and then draw shapes from it.
    // This invokes the shader program, which accesses the vertex buffers.
    d.bindIdx();
    context->glDrawElements(d.drawMode(), d.elemCount(), GL_UNSIGNED_INT, 0);

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
}

void ShadowMapShader::drawFlat(Drawable &d, int textureSlot)
{
    if (!d.initialized()) return;

    useMe();
    ++DrawCalls;

    if(d.elemCount() < 0) {
        throw std::out_of_range("Attempting to draw a drawable with m_count of " + std::to_string(d.elemCount()) + "!");
    }

    if(unifLightDepthMap != -1)
    {
        context->glUniform1i(unifLightDepthMap, textureSlot);
    }

    if (unifPosMap != -1)
    {
        context->glUniform1i(unifPosMap, textureSlot + 1);
    }

    if (attrPos != -1 && d.bindPos()) {
        context->glEnableVertexAttribArray(attrPos);
        context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 0, NULL);
    }
    if (attrUV != -1 && d.bindUV()) {
        context->glEnableVertexAttribArray(attrUV);
        context->glVertexAttribPointer(attrUV, 2, GL_FLOAT, false, 0, NULL);
    }

    // Bind the index buffer and then draw shapes from it.
    // This invokes the shader program, which accesses the vertex buffers.
    d.bindIdx();
    context->glDrawElements(d.drawMode(), d.elemCount(), GL_UNSIGNED_INT, 0);

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrUV != -1) context->glDisableVertexAttribArray(attrUV);

    context->printGLErrorLog();
}


