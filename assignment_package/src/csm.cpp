#include "csm.h"

CascadedShadowMap::CascadedShadowMap(OpenGLContext* context)
    : mp_context(context), lightFBO(-1), shadowFBO(-1),
      lightDepthMaps(-1), shadowMap(-1), shadowDepth(-1),
      progLightDepth(context), progShadowMap(context)
{}

void CascadedShadowMap::initialize()
{
    // light depth buffer initialization
    progLightDepth.create(":/glsl/lightdepth.vert.glsl", ":/glsl/lightdepth.frag.glsl");
    mp_context->glGenFramebuffers(1, &lightFBO);
    mp_context->glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);

    glGenTextures(1, &lightDepthMaps);
    glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
    mp_context->glTexImage3D(
        GL_TEXTURE_2D_ARRAY,
        0,
        GL_DEPTH_COMPONENT16,
        CSM_RES,
        CSM_RES,
        LayerNumber,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        nullptr);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);
    mp_context->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, lightDepthMaps, 0);


    // shadow map initialization
    progShadowMap.create(":/glsl/shadowmap.vert.glsl", ":/glsl/shadowmap.frag.glsl");
    mp_context->glGenFramebuffers(1, &shadowFBO);
    mp_context->glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    unsigned int width = mp_context->width();
    unsigned int height = mp_context->height();
    float pixelRatio = mp_context->devicePixelRatio();

    // depth buffer init
    mp_context->glGenRenderbuffers(1, &shadowDepth);
    mp_context->glBindRenderbuffer(GL_RENDERBUFFER, shadowDepth);
    mp_context->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                width * pixelRatio,
                height * pixelRatio);
    mp_context->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, shadowDepth);

    // output texture
    mp_context->glGenTextures(1, &shadowMap);
    mp_context->glBindTexture(GL_TEXTURE_2D, shadowMap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width * pixelRatio, height * pixelRatio, 0, GL_RED, GL_FLOAT, (void*)0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_RED};
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
    mp_context->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, shadowMap, 0);

    mp_context->printGLErrorLog();
}

void CascadedShadowMap::bindLightDepthBuffer(int layer)
{
    mp_context->glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
    mp_context->glFramebufferTextureLayer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        lightDepthMaps, 0, layer);
}

void CascadedShadowMap::bindLightDepthTexture(int slot)
{
    mp_context->glActiveTexture(GL_TEXTURE0 + slot);
    mp_context->glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
}

void CascadedShadowMap::bindShadowFBO()
{
    mp_context->glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
}

void CascadedShadowMap::bindShadowMap(int slot)
{
    mp_context->glActiveTexture(GL_TEXTURE0 + slot);
    mp_context->glBindTexture(GL_TEXTURE_2D, shadowMap);
}
