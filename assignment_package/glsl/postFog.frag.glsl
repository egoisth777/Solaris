#version 330
// Fog Effect
// Reference:
//  https://iquilezles.org/articles/fog/
in vec2 fs_UV;

out vec4 color;

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_PositionMap;
uniform sampler2D u_SkyPureMap;
uniform sampler2D u_FogIntensity;

uniform mat4 u_View;
uniform vec2 u_NearFarClip;
uniform vec3 u_CameraPos;


uniform bool u_EnableShader;
uniform bool u_EnableShaderFeature1;    // Enable screen-spaced-multiple-scattering

const int   SAMPLE_LEVEL_COUNT  = 10;   // fog level
const float SAMPLE_RANGE        = 3;    // spatialParameter
const float SAMPLE_INTENSITY    = 6;    // rangeParameter


vec4 getViewPos(vec2 uv){
    return u_View * texture(u_PositionMap, uv);
}

float getDepth(float z){
    /// Note that z is negative in view space
    return clamp((u_NearFarClip.x + abs(z)) / (u_NearFarClip.y - u_NearFarClip.x),
                 0,
                 1.0f);
}

float calculateLevel(float val, float levelCount){
    float lower = floor(val * levelCount) / levelCount;
    float lowerDist = val - lower;

    float upper = ceil(val * levelCount) / levelCount;
    float upperDist = upper - val;

    return upperDist > lowerDist ? upper : lower;
}

vec3 getTintRgb(vec2 uv){
    vec4 albedo = texture(u_AlbedoMap, uv);
    vec3 fogColor = texture(u_SkyPureMap, uv).rgb;

    if(albedo.a <= 0){
        return fogColor;
    }
    return mix(albedo.rgb,
               fogColor,
               texture(u_FogIntensity, uv).x);
}

float intensity(vec3 inColor) {
    return length(inColor);
}

vec3 bilaterialFilter(float sigmaS, float sigmaL){
    const float PI = 3.14159f;
    float facSsquare = 1./(2.*sigmaS*sigmaS);
    float facLsquare = 1./(2.*sigmaL*sigmaL);

    float normalizationFactor   = 0;
    vec3  sumColor              = vec3(0);
    float halfSize  = sigmaS * 2;
    halfSize = 8.f;
    ivec2 dimension = textureSize(u_AlbedoMap, 0);

    float currentIntensity = intensity(getTintRgb(fs_UV));

    for (float i = -halfSize; i <= halfSize; i ++){
        for (float j = -halfSize; j <= halfSize; j ++){
            vec2 pos = vec2(i, j);
            vec3 offsetColor = getTintRgb(fs_UV + pos / dimension);
            float weight = pow(halfSize - abs(i) + halfSize - abs(j), 2.);

            float distS = length(pos);
            float distL = intensity(offsetColor) - currentIntensity;    

            float gS = facSsquare / PI * exp(-facSsquare * float(distS * distS));
            float gL = facLsquare / PI * exp(-facLsquare * float(distL * distL));
            float g = gS * gL * weight;

            normalizationFactor += g;
            sumColor += offsetColor * g;
        }
    }
    return sumColor / normalizationFactor;
}

void main() {
    color = texture(u_AlbedoMap, fs_UV);
    if(color.a <= 0 || !u_EnableShader){
        return;
    }

    if(!u_EnableShaderFeature1){
        color.rgb = getTintRgb(fs_UV);
        return;
    }

    float fogIntensity = texture(u_FogIntensity, fs_UV).x;
    float intensityLevel = calculateLevel(fogIntensity, SAMPLE_LEVEL_COUNT);
    if(intensityLevel <= 0.){
        return;
    }
    color.rgb = bilaterialFilter(SAMPLE_RANGE * intensityLevel,
                                 SAMPLE_INTENSITY * intensityLevel);
}
