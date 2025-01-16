#include "shaderprograms/compositeshader.h"

CompositeShader::CompositeShader(OpenGLContext* context)
    : ShaderProgram(context), attrPos(-1), attrUV(-1),
      unifModel(-1), unifModelInvTr(-1), unifView(-1),
      unifViewProj(-1), unifLightDir(-1),
      unifAlbedoMap(-1), unifNormalMap(-1),
      unifPosMap(-1), unifShaderMap(-1),
      unifSkyMap(-1)
{}

void CompositeShader::setupMemberVars()
{
    attrPos = context->glGetAttribLocation(prog, "vs_Pos");
    attrUV  = context->glGetAttribLocation(prog, "vs_UV");

    unifModel      = context->glGetUniformLocation(prog, "u_Model");
    unifModelInvTr = context->glGetUniformLocation(prog, "u_ModelInvTr");
    unifView       = context->glGetUniformLocation(prog, "u_View");
    unifViewProj   = context->glGetUniformLocation(prog, "u_ViewProj");

    unifAlbedoMap  = context->glGetUniformLocation(prog, "u_AlbedoMap");
    unifNormalMap  = context->glGetUniformLocation(prog, "u_NormalMap");
    unifPosMap     = context->glGetUniformLocation(prog, "u_PosMap");
    unifShaderMap  = context->glGetUniformLocation(prog, "u_ShadowMap");
    unifSSAOMap    = context->glGetUniformLocation(prog, "u_SSAOMap");
    unifSkyMap     = context->glGetUniformLocation(prog, "u_SkyMap");

    unifTime       = context->glGetUniformLocation(prog, "u_Time");
    unifLightDir   = context->glGetUniformLocation(prog, "u_LightDir");
}

void CompositeShader::setModelMatrix(const glm::mat4 &model)
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

    if (unifModelInvTr != -1) {
        glm::mat4 modelinvtr = glm::inverse(glm::transpose(model));
        // Pass a 4x4 matrix into a uniform variable in our shader
        // Handle to the matrix variable on the GPU
        context->glUniformMatrix4fv(unifModelInvTr,
                                    // How many matrices to pass
                                    1,
                                    // Transpose the matrix? OpenGL uses column-major, so no.
                                    GL_FALSE,
                                    // Pointer to the first element of the matrix
                                    &modelinvtr[0][0]);
    }
}

void CompositeShader::setViewMatrix(const glm::mat4 &v)
{
    useMe();

    if(unifView != -1)
    {
        context->glUniformMatrix4fv(unifView,
                                    // How many matrices to pass
                                    1,
                                    // Transpose the matrix? OpenGL uses column-major, so no.
                                    GL_FALSE,
                                    // Pointer to the first element of the matrix
                                    &v[0][0]);
    }
}

void CompositeShader::setViewProjMatrix(const glm::mat4 &vp)
{
    useMe();

    if(unifViewProj != -1)
    {
        context->glUniformMatrix4fv(unifViewProj,
                                    // How many matrices to pass
                                    1,
                                    // Transpose the matrix? OpenGL uses column-major, so no.
                                    GL_FALSE,
                                    // Pointer to the first element of the matrix
                                    &vp[0][0]);
    }
}

void CompositeShader::setLightDir()
{
    useMe();
    if(unifLightDir != -1) {
        glm::vec3 dir = MGlobalInstance::Get->getLightAngle();
        context->glUniform3fv(unifLightDir, 1, &dir[0]);
        context->printGLErrorLog();
    }
}

void CompositeShader::draw(Drawable &d, int textureSlot)
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

    if(unifNormalMap != -1)
    {
        context->glUniform1i(unifNormalMap, textureSlot + 1);
    }

    if(unifPosMap != -1)
    {
        context->glUniform1i(unifPosMap, textureSlot + 2);
    }

    if(unifShaderMap != -1)
    {
        context->glUniform1i(unifShaderMap, textureSlot + 3);
    }

    if(unifShaderMap != -1)
    {
        context->glUniform1i(unifSSAOMap, textureSlot + 4);
    }

    if(unifSkyMap != -1)
    {
        context->glUniform1i(unifSkyMap, textureSlot + 5);
    }

    // Each of the following blocks checks that:
    //   * This shader has this attribute, and
    //   * This Drawable has a vertex buffer for this attribute.
    // If so, it binds the appropriate buffers to each attribute.

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
