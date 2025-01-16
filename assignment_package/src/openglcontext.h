#pragma once

#include <QOpenGLWidget>
#include <QTimer>
#include <QOpenGLExtraFunctions>


class OpenGLContext
    : public QOpenGLWidget,
      public QOpenGLExtraFunctions
{

public:
    OpenGLContext(QWidget *parent);
    ~OpenGLContext();

    void debugContextVersion();
    void printGLErrorLog();
    void printLinkInfoLog(int prog);
    void printShaderInfoLog(int shader);

    // Virtuals
protected:
    /// Determine the actual performce when getting an GL error
    /// Defaultly quit the program
    virtual void onGLGetError(GLenum);
    virtual void onLinkGetError();
    virtual void onShaderGetError();
};
