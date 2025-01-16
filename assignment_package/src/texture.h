#ifndef TEXTURE_H
#define TEXTURE_H

#include <openglcontext.h>
#include <memory>

// frome ShaderFun assignment
class Texture
{
public:
    Texture(OpenGLContext* context);
    ~Texture();

    void create(const char *texturePath);
    void create(unsigned int width, unsigned int height,
                GLint iFormat = GL_RGB, GLenum format = GL_RGB,
                GLenum type = GL_UNSIGNED_BYTE, GLint filter = GL_NEAREST,
                GLint clamp = GL_CLAMP_TO_EDGE);
    void load(int texSlot);
    void bind(int texSlot);
    GLuint getTextureHandle();

private:
    OpenGLContext* context;
    GLuint m_textureHandle;
    std::shared_ptr<QImage> m_textureImage;
    unsigned int width, height;
    float pixelRatio;
};

#endif // TEXTURE_H
