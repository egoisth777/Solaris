#ifndef QUAD_H
#define QUAD_H

#include "drawable.h"

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

// code from assignment 4
class Quad : public Drawable
{
public:
    Quad(OpenGLContext* context);
    void createVBOdata() override;
};

#endif // QUAD_H
