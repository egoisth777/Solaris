#ifndef GBUFFERMODEL_H
#define GBUFFERMODEL_H

#include <smartpointerhelp.h>
#include "texture.h"
#include "framebuffer.h"

struct GBuffer{
    uPtr<FrameBuffer> gBuffer  = nullptr;
    uPtr<Texture> gTexPosition = nullptr;
    uPtr<Texture> gTexViewPosition = nullptr;       // Pos in view space
    uPtr<Texture> gTexVelocity = nullptr;
    uPtr<Texture> gTexNormal   = nullptr;           // Norm
    uPtr<Texture> gTexViewNormal   = nullptr;       // Norm in view space
    uPtr<Texture> gTexAlbedo   = nullptr;
};

#endif // GBUFFERMODEL_H
