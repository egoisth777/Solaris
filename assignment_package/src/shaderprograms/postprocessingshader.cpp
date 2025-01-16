#include "postprocessingshader.h"

PostProcessingShader::PostProcessingShader(OpenGLContext* context)
    : ShaderProgram(context), attrPos(-1), attrUV(-1),
    unifDimensions(-1), unifAlbedoMap(-1), unifPositionMap(-1), unifViewNormMap(-1)
{}

void PostProcessingShader::setupMemberVars()
{
    // Get the handles to the variables stored in our shaders
    // See shaderprogram.h for more information about these variables

    attrPos = context->glGetAttribLocation(prog, "vs_Pos");
    attrUV  = context->glGetAttribLocation(prog, "vs_UV");

    unifDimensions  = context->glGetUniformLocation(prog, "u_Dimensions");
    unifAlbedoMap   = context->glGetUniformLocation(prog, "u_AlbedoMap");
    unifPositionMap = context->glGetUniformLocation(prog, "u_PositionMap");
    unifViewNormMap = context->glGetUniformLocation(prog, "u_ViewNormalMap");
    unifSkyMap      = context->glGetUniformLocation(prog, "u_SkyMap");
    unifSkyPureMap  = context->glGetUniformLocation(prog, "u_SkyPureMap");
    unifFogIntensityMap  = context->glGetUniformLocation(prog, "u_FogIntensity");
    unifTime        = context->glGetUniformLocation(prog, "u_Time");

    unifViewProj    = context->glGetUniformLocation(prog, "u_ViewProj");
    unifView        = context->glGetUniformLocation(prog, "u_View");
    unifProj        = context->glGetUniformLocation(prog, "u_Proj");
    unifCameraPos   = context->glGetUniformLocation(prog, "u_CameraPos");
    unifNearFarClip = context->glGetUniformLocation(prog, "u_NearFarClip");
}

void PostProcessingShader::draw(Drawable &d, int texSlot)
{
    useMe();
    ++DrawCalls;

    // Set our "renderedTexture" sampler to user Texture Unit 0
    context->glUniform1i(unifAlbedoMap, texSlot);

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

void PostProcessingShader::setDimensions(glm::ivec2 dims)
{
    useMe();

    if(unifDimensions != -1)
    {
        context->glUniform2i(unifDimensions, dims.x, dims.y);
    }
}

void PostProcessingShader::setUnifPositionTex(int textureSlot)
{
    useMe();

    if(unifPositionMap == -1){
        return;
    }
    context->glUniform1i(unifPositionMap, textureSlot);
}

void PostProcessingShader::setUnifViewNormTex(int textureSlot)
{
    useMe();

    if(unifViewNormMap == -1){
        return;
    }
    context->glUniform1i(unifViewNormMap, textureSlot);
}

void PostProcessingShader::setUnifSkyTex(int textureSlot)
{
    useMe();

    if(unifSkyMap == -1){
        return;
    }
    context->glUniform1i(unifSkyMap, textureSlot);
}

void PostProcessingShader::setUnifSkyPureTex(int textureSlot)
{
    useMe();

    if(unifSkyPureMap == -1){
        return;
    }
    context->glUniform1i(unifSkyPureMap, textureSlot);
}

void PostProcessingShader::setUnifFogIntensityTex(int textureSlot)
{
    useMe();

    if(unifFogIntensityMap == -1){
        return;
    }
    context->glUniform1i(unifFogIntensityMap, textureSlot);
}

void PostProcessingShader::setUnifProjMatrix(const glm::mat4 &p)
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

void PostProcessingShader::setUnifViewMatrix(const glm::mat4 &v)
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

void PostProcessingShader::setUnifViewProjMatrix(const glm::mat4 &vp)
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

void PostProcessingShader::setUnifCameraPos(const glm::vec3 &p)
{
    useMe();

    if(unifCameraPos != -1)
    {
        context->glUniform3fv(unifCameraPos, 1, &p[0]);
    }
}

void PostProcessingShader::setUnifNearFarClip(const glm::vec2 &c)
{
    useMe();

    if(unifNearFarClip == -1){
        return;
    }
    context->glUniform2fv(unifNearFarClip, 1, &c[0]);
}
