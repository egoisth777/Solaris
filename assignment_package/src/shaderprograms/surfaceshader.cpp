#include "surfaceshader.h"

SurfaceShader::SurfaceShader(OpenGLContext* context)
    : ShaderProgram(context), attrPos(-1), attrNor(-1), attrUV(-1),
      attrCol(-1), attrType(-1),
      unifModel(-1), unifModelInvTr(-1), unifViewProj(-1),
      unifAlbedoMap(-1),  unifNormalMap(-1)
{}

void SurfaceShader::setupMemberVars()
{
    // Get the handles to the variables stored in our shaders
    // See shaderprogram.h for more information about these variables

    attrPos  = context->glGetAttribLocation(prog, "vs_Pos");
    attrNor  = context->glGetAttribLocation(prog, "vs_Nor");
    attrUV   = context->glGetAttribLocation(prog, "vs_UV");
    attrCol  = context->glGetAttribLocation(prog, "vs_Col");
    attrType = context->glGetAttribLocation(prog, "vs_Type");

    unifModel      = context->glGetUniformLocation(prog, "u_Model");
    unifModelInvTr = context->glGetUniformLocation(prog, "u_ModelInvTr");
    unifViewProj   = context->glGetUniformLocation(prog, "u_ViewProj");
    unifView   = context->glGetUniformLocation(prog, "u_View");
    unifProj   = context->glGetUniformLocation(prog, "u_Proj");
    unifViewProjLastFrame   = context->glGetUniformLocation(prog, "u_ViewProjLastFrame");

    unifAlbedoMap  = context->glGetUniformLocation(prog, "u_AlbedoMap");
    unifNormalMap  = context->glGetUniformLocation(prog, "u_NormalMap");
    unifTime       = context->glGetUniformLocation(prog, "u_Time");
}

void SurfaceShader::setModelMatrix(const glm::mat4 &model)
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

void SurfaceShader::setViewProjMatrix(const glm::mat4 &vp)
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

void SurfaceShader::setViewProjMatrixLastFrame(const glm::mat4 &vp)
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

void SurfaceShader::setViewMatrix(const glm::mat4 &v)
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

void SurfaceShader::setProjMatrix(const glm::mat4 &p)
{
    useMe();

    if(unifProj != -1)
    {
        context->glUniformMatrix4fv(unifProj,
                                    // How many matrices to pass
                                    1,
                                    // Transpose the matrix? OpenGL uses column-major, so no.
                                    GL_FALSE,
                                    // Pointer to the first element of the matrix
                                    &p[0][0]);
    }
}

void SurfaceShader::draw(Drawable &d, int textureSlot)
{
    if (!d.initialized()) return;
    // entry for flat shader
    if (d.drawMode() == GL_LINES)
        return drawFlat(d);

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

        if (attrType != -1)
        {
            context->glEnableVertexAttribArray(attrType);
            context->glVertexAttribIPointer(attrType, 1, GL_INT, sizeof(Drawable::VertexData), (void*)(sizeof(float)*10));
        }
    }

    // Bind the index buffer and then draw shapes from it.
    // This invokes the shader program, which accesses the vertex buffers.
    d.bindIdx();
    context->glDrawElements(d.drawMode(), d.elemCount(), GL_UNSIGNED_INT, 0);

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);
    if (attrUV != -1) context->glDisableVertexAttribArray(attrUV);
    if (attrType != -1) context->glDisableVertexAttribArray(attrType);
}

void SurfaceShader::drawTransp(Drawable &d, int textureSlot)
{
    if (d.traElemCount() < 1) return;
    useMe();
    ++DrawCalls;

    if(d.traElemCount() < 0) {
        throw std::out_of_range("Attempting to draw a drawable with m_traCnt of " + std::to_string(d.elemCount()) + "!");
    }

    if(unifAlbedoMap != -1)
    {
        context->glUniform1i(unifAlbedoMap, textureSlot);
    }

    if(unifNormalMap != -1)
    {
        context->glUniform1i(unifNormalMap, textureSlot + 1);
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

        if (attrUV != -1)
        {
            context->glEnableVertexAttribArray(attrUV);
            context->glVertexAttribPointer(attrUV, 2, GL_FLOAT, false, sizeof(Drawable::VertexData), (void*)(sizeof(float)*8));
        }

        if (attrType != -1)
        {
            context->glEnableVertexAttribArray(attrType);
            context->glVertexAttribPointer(attrType, 1, GL_FLOAT, false, sizeof(Drawable::VertexData), (void*)(sizeof(float)*10));
        }
    }

    // Bind the index buffer and then draw shapes from it.
    // This invokes the shader program, which accesses the vertex buffers.
    d.bindTraIdx();
    context->glDrawElements(d.drawMode(), d.traElemCount(), GL_UNSIGNED_INT, 0);

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);
    if (attrUV != -1) context->glDisableVertexAttribArray(attrUV);
    if (attrType != -1) context->glDisableVertexAttribArray(attrType);
}


void SurfaceShader::drawFlat(Drawable &d)
{
    useMe();

    if(d.elemCount() < 0) {
        throw std::out_of_range("Attempting to draw a drawable with m_count of " + std::to_string(d.elemCount()) + "!");
    }

    // Each of the following blocks checks that:
    //   * This shader has this attribute, and
    //   * This Drawable has a vertex buffer for this attribute.
    // If so, it binds the appropriate buffers to each attribute.

    // Remember, by calling bindPos(), we call
    // glBindBuffer on the Drawable's VBO for vertex position,
    // meaning that glVertexAttribPointer associates vs_Pos
    // (referred to by attrPos) with that VBO
    if (attrPos != -1 && d.bindPos()) {
        context->glEnableVertexAttribArray(attrPos);
        context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 0, NULL);
    }

    if (attrNor != -1 && d.bindNor()) {
        context->glEnableVertexAttribArray(attrNor);
        context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, 0, NULL);
    }

    if (attrCol != -1 && d.bindCol()) {
        context->glEnableVertexAttribArray(attrCol);
        context->glVertexAttribPointer(attrCol, 4, GL_FLOAT, false, 0, NULL);
    }

    // Bind the index buffer and then draw shapes from it.
    // This invokes the shader program, which accesses the vertex buffers.
    d.bindIdx();
    context->glDrawElements(d.drawMode(), d.elemCount(), GL_UNSIGNED_INT, 0);

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);
    if (attrCol != -1) context->glDisableVertexAttribArray(attrCol);

    context->printGLErrorLog();
}
