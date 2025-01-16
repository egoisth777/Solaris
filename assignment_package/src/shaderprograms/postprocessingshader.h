#ifndef POSTPROCESSINGSHADER_H
#define POSTPROCESSINGSHADER_H

#include "shaderprogram.h"

class PostProcessingShader : public ShaderProgram
{
public:
    int attrPos; // A handle for the "in" vec4 representing vertex position in the vertex shader
    int attrUV;  // A handle for the "in" vec4 representing vertex uv coord in the vertex shader

    int unifDimensions;
    int unifAlbedoMap;
    int unifPositionMap;
    int unifViewNormMap;
    int unifSkyMap;
    int unifSkyPureMap;
    int unifFogIntensityMap;

    int unifViewProj;
    int unifView;
    int unifProj;
    int unifCameraPos;

    int unifNearFarClip;


public:
    PostProcessingShader(OpenGLContext* context);
    // Sets up shader-specific handles
    virtual void setupMemberVars() override;
    // Draw the given object to our screen using this ShaderProgram's shaders
    virtual void draw(Drawable &d, int textureSlot = 0) override;

    void setDimensions(glm::ivec2 dims);
    void setUnifPositionTex(int textureSlot);
    void setUnifViewNormTex(int textureSlot);
    void setUnifSkyTex(int textureSlot);
    void setUnifSkyPureTex(int textureSlot);
    void setUnifFogIntensityTex(int textureSlot);
    void setUnifProjMatrix(const glm::mat4&);
    void setUnifViewMatrix(const glm::mat4&);
    void setUnifViewProjMatrix(const glm::mat4 &vp);
    void setUnifCameraPos(const glm::vec3 &p);
    void setUnifNearFarClip(const glm::vec2&);
};

#endif // POSTPROCESSINGSHADER_H
