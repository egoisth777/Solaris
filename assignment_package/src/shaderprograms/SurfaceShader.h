#ifndef SURFACESHADER_H
#define SURFACESHADER_H

#include "shaderprogram.h"

class SurfaceShader : public ShaderProgram
{
private:
    int attrPos; // A handle for the "in" vec4 representing vertex position in the vertex shader
    int attrNor; // A handle for the "in" vec4 representing vertex normal in the vertex shader
    int attrUV;  // A handle for the "in" vec4 representing vertex uv coord in the vertex shader
    int attrCol; // A handle for the "in" vec4 representing vertex color in the vertex shader
    int attrType;

    int unifModel; // A handle for the "uniform" mat4 representing model matrix in the vertex shader
    int unifModelInvTr; // A handle for the "uniform" mat4 representing inverse transpose of the model matrix in the vertex shader
    int unifView;
    int unifProj;
    int unifViewProj; // A handle for the "uniform" mat4 representing combined projection and view matrices in the vertex shader
    int unifViewProjLastFrame;  // A handle for the "uniform" mat4 representing combined projection and view matrices last frame

    int unifAlbedoMap; // A handle to the "uniform" sampler2D that will be used to read the texture containing the scene render
    int unifNormalMap;


public:
    SurfaceShader(OpenGLContext* context);
    // Sets up shader-specific handles
    virtual void setupMemberVars() override;
    // Pass the given model matrix to this shader on the GPU
    void setModelMatrix(const glm::mat4 &model);
    // Pass the given Projection * View matrix to this shader on the GPU
    void setViewProjMatrix(const glm::mat4 &vp);
    void setViewProjMatrixLastFrame(const glm::mat4 &vp);
    void setViewMatrix(const glm::mat4 &v);
    void setProjMatrix(const glm::mat4 &p);

    // Pass the given color to this shader on the GPU
    void setGeometryColor(glm::vec4 color);
    // Draw the given object to our screen using this ShaderProgram's shaders
    virtual void draw(Drawable &d, int textureSlot = 0) override;
    virtual void drawTransp(Drawable &d, int textureSlot = 0) override;

protected:
    void drawFlat(Drawable &d);
};

#endif // SURFACESHADER_H
