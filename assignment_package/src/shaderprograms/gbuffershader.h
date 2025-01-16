#ifndef GBUFFERSHADER_H
#define GBUFFERSHADER_H

#include "surfaceshader.h"



class GBufferShader : public SurfaceShader
{
public:
    GBufferShader(OpenGLContext* context);
};

#endif // GBUFFERSHADER_H
