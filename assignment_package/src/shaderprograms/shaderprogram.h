#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <openglcontext.h>
#include <glm_includes.h>
#include <glm/glm.hpp>

#include "drawable.h"


class ShaderProgram
{
public:
    inline static int DrawCalls = 0;
    GLuint vertShader; // A handle for the vertex shader stored in this shader program
    GLuint fragShader; // A handle for the fragment shader stored in this shader program
    GLuint prog;       // A handle for the linked shader program stored in this class

    int unifTime; // A handle for the "uniform" float representing time in the shader

    /// Debugger & Profiling
    int unifEnableShader;
    int unifEnableShaderFeature1;
    int unifEnableShaderFeature2;
    int unifEnableShaderFeature3;

public:
    ShaderProgram(OpenGLContext* context);
    // Sets up the requisite GL data and shaders from the given .glsl files
    void create(const char *vertfile, const char *fragfile);
    // Tells our OpenGL context to use this shader to draw things
    void useMe();
    // Sets up shader-specific handles
    virtual void setupMemberVars() = 0;
    // Draw the given object to our screen using this ShaderProgram's shaders
    virtual void draw(Drawable &d, int textureSlot = 0) = 0;
    virtual void drawTransp(Drawable &d, int textureSlot = 0) {return;};
    void drawInterleaved(Drawable &d);
    // Draw the given object to our screen multiple times using instanced rendering
    void drawInstanced(InstancedDrawable &d);
    // Utility function used in create()
    char* textFileRead(const char*);
    // Utility function that prints any shader compilation errors to the console
    void printShaderInfoLog(int shader);
    // Utility function that prints any shader linking errors to the console
    void printLinkInfoLog(int prog);

    void setTime(int64_t t);

    void setUnifEnableShader(bool);
    void setUnifEnableShaderFeature1(bool);
    void setUnifEnableShaderFeature2(bool);
    void setUnifEnableShaderFeature3(bool);

    QString qTextFileRead(const char*);

protected:
    OpenGLContext* context;   // Since Qt's OpenGL support is done through classes like QOpenGLFunctions_3_2_Core,
                            // we need to pass our OpenGL context to the Drawable in order to call GL functions
                            // from within this class.
};


#endif // SHADERPROGRAM_H
