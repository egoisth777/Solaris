#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <openglcontext.h>
#include <glm_includes.h>
#include <vector>

class FrameBuffer
{
private:
    OpenGLContext* mp_context;
    GLuint m_frameBuffer;
    GLuint m_depthBuffer;
    std::vector<GLenum> drawBuffers;

    bool m_created;
    unsigned int width, height;
    float pixelRatio;

public:
    FrameBuffer(OpenGLContext* context, unsigned int width, unsigned int height);
    ~FrameBuffer();
    void resize(unsigned int width, unsigned int height);
    void genBuffer();
    void create();
    void createDepthBuffer();
    void destroy();
    void bindFrameBuffer();
    void bindTexture(GLuint texture, int slot);
    void bindDepthTexture(int slot);
    void updateBindTexture(GLuint texture, unsigned int slot = 0);
    int  getDrawBufferCount() const;
    GLuint getFrameBufferHandle() const;

private:
    void checkAssignmentError();
};

#endif // FRAMEBUFFER_H
