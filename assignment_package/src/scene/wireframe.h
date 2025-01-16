#ifndef WIREFRAME_H
#define WIREFRAME_H

#include "drawable.h"
#include <glm_includes.h>

class WireFrame : public Drawable
{
public:
    WireFrame(OpenGLContext* context) : Drawable(context){}
    virtual ~WireFrame() override;
    void createVBOdata() override;
    GLenum drawMode() override;

    glm::vec4 offset;
};

#endif // WIREFRAME_H
