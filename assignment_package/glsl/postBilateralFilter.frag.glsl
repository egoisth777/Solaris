# version 330

/// Bilateral Filter Blur
/// Reference:
///     https://www.shadertoy.com/view/4dfGDH
///     https://github.com/tranvansang/bilateral-filter
///     https://people.csail.mit.edu/sparis/publi/2009/fntcgv/Paris_09_Bilateral_filtering.pdf

uniform sampler2D   u_AlbedoMap;

// As the range parameter σr increases,
// the bilateral filter gradually approximates Gaussian convolution
uniform float       spatialParameter;

// Increasing the spatial parameter σs smooths larger features
uniform float       rangeParameter;

in vec2 fs_UV;
layout (location = 0) out vec4 color;

const float PI = 3.14159f;

float intensity(in vec4 color) {
    return length(color.xyz);
}

void main()
{
    float sigmaS = spatialParameter;
    float sigmaL = rangeParameter;

    sigmaS = max(0.1f, sigmaS);
    sigmaL = max(0.1f, sigmaL);

    float facSsquare = 1./(2.*sigmaS*sigmaS);
    float facLsquare = 1./(2.*sigmaL*sigmaL);

    float normalizationFactor   = 0;
    vec4  sumColor              = vec4(0);
    float halfSize  = sigmaS * 2;
    ivec2 dimension = textureSize(u_AlbedoMap, 0);

    float currentIntensity = intensity(texture2D(u_AlbedoMap,fs_UV));

    for (float i = -halfSize; i <= halfSize; i ++){
        for (float j = -halfSize; j <= halfSize; j ++){
            vec2 pos = vec2(i, j);
            vec4 offsetColor = texture2D(u_AlbedoMap, fs_UV + pos / dimension);

            float distS = length(pos);
            float distL = intensity(offsetColor) - currentIntensity;

            float gS = facSsquare / PI * exp(-facSsquare * float(distS * distS));
            float gL = facLsquare / PI * exp(-facLsquare * float(distL * distL));
            float g = gS * gL;

            normalizationFactor += g;
            sumColor += offsetColor * g;
        }
    }

    color = sumColor / normalizationFactor;
}
