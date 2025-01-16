#include "framebuffer.h"
#include "iostream"


FrameBuffer::FrameBuffer(OpenGLContext* context, unsigned int width, unsigned int height)
    : mp_context(context), width(width), height(height),
      pixelRatio(context->devicePixelRatioF())
{}

FrameBuffer::~FrameBuffer()
{
    destroy();
}

void FrameBuffer::genBuffer()
{
    mp_context->glGenFramebuffers(1, &m_frameBuffer);
}

void FrameBuffer::create()
{
    mp_context->glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    mp_context->glDrawBuffers(drawBuffers.size(), drawBuffers.data());

    checkAssignmentError();
    if(mp_context->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Frame buffer did not initialize correctly..." << std::endl;
        mp_context->printGLErrorLog();
    }
}

void FrameBuffer::createDepthBuffer()
{
    mp_context->glGenRenderbuffers(1, &m_depthBuffer);
    mp_context->glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
    mp_context->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width * pixelRatio, height * pixelRatio);
    mp_context->glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    mp_context->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);
}

void FrameBuffer::bindTexture(GLuint texture, int slot)
{
    if (m_frameBuffer != -1)
    {
        mp_context->glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
        mp_context->glBindTexture(GL_TEXTURE_2D, texture);

        mp_context->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot, texture, 0);
        drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + slot);
    }
}


void FrameBuffer::bindDepthTexture(int slot)
{
    if (m_depthBuffer != -1)
    {
        mp_context->glActiveTexture(GL_TEXTURE0 + slot);
        mp_context->glBindTexture(GL_TEXTURE_2D, m_depthBuffer);
    }
}

void FrameBuffer::updateBindTexture(GLuint texture, unsigned int slot)
{
    if (m_frameBuffer == -1){
        return;
    }

    // Do nothing if the slot is not bound previously
    bool doUpdate = false;
    for(auto slotNum : drawBuffers){
        if(slotNum == GL_COLOR_ATTACHMENT0 + slot){
            doUpdate = true;
            break;
        }
    }

    if(!doUpdate){
        return;
    }

    mp_context->glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    mp_context->glBindTexture(GL_TEXTURE_2D, texture);
    mp_context->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot, texture, 0);
    //mp_context->glDrawBuffers(drawBuffers.size(), drawBuffers.data());
    create();
}
int FrameBuffer::getDrawBufferCount() const
{
    return drawBuffers.size();
}

void FrameBuffer::checkAssignmentError()
{
    int assignmentCount = getDrawBufferCount();
    GLint maxDrawBuffers;
    mp_context->glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffers);

    if(maxDrawBuffers < assignmentCount){
        std::cerr << "The hardware max-draw-buffers is: " << maxDrawBuffers
                  << ", actual used: " << assignmentCount << std::endl;;
        std::cerr << "GL_MAX_DRAW_BUFFERS EXCEEDED!" << std::endl;
        throw;
    }
}

void FrameBuffer::bindFrameBuffer()
{
    mp_context->glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
}

GLuint FrameBuffer::getFrameBufferHandle() const
{
    return m_frameBuffer;
}

void FrameBuffer::destroy()
{
    mp_context->glDeleteFramebuffers(1, &m_frameBuffer);
}
