#ifndef COMPOSITESHADER_H
#define COMPOSITESHADER_H

#include "shaderprograms/shaderprogram.h"
#include "globalinstance.h"

class CompositeShader : public ShaderProgram
{
private:
    int attrPos; // A handle for the "in" vec4 representing vertex position in the vertex shader
    int attrUV;  // A handle for the "in" vec4 representing vertex uv coord in the vertex shader

    int unifModel; // A handle for the "uniform" mat4 representing model matrix in the vertex shader
    int unifModelInvTr; // A handle for the "uniform" mat4 representing inverse transpose of the model matrix in the vertex shader
    int unifView;
    int unifViewProj; // A handle for the "uniform" mat4 representing combined projection and view matrices in the vertex shader
    int unifLightDir;

    // textures
    int unifAlbedoMap; // A handle to the "uniform" sampler2D that will be used to read the texture containing the scene render
    int unifNormalMap;
    int unifPosMap;
    int unifShaderMap;
    int unifSSAOMap;
    int unifSkyMap;

public:
    CompositeShader(OpenGLContext* context);
    virtual void setupMemberVars() override;
    void setModelMatrix(const glm::mat4 &model);
    void setViewProjMatrix(const glm::mat4 &vp);
    void setViewMatrix(const glm::mat4 &v);
    void setLightDir();
    virtual void draw(Drawable &d, int textureSlot = 0) override;
};

#endif // COMPOSITESHADER_H
