#version 330
// Outline Effect

in vec2 fs_UV;

out vec4 color;

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_PositionMap;
uniform sampler2D u_ViewNormalMap;

uniform mat4 u_View;
uniform vec2 u_NearFarClip;

const int   OUTLINE_WIDTH = 2;

/// The outline reaches max when the view-space
/// depth-difference between 2 pixel is larger than 2.5
const vec2  OUTLINE_INTENSITY_FACTOR = vec2(.2f, 2.5f);

/// The outline will be thinner and thinner until invisiable when reaching 0.4 far-clip
const vec2  OUTLINE_DEPTH_FACTOR = vec2(0, .4);

/// Define this to enable culling with norm, but the effect is not very good
// #define UseNormCull

vec4 getViewPos(vec2 uv){
    return u_View * texture(u_PositionMap, uv);
}

vec3 darkenColor(vec3 color){
    return color * vec3(0.324, 0.063, 0.099);
}


float getDepth(float z){
    /// Note that z is negative in view space
    return clamp((u_NearFarClip.x + abs(z)) / (u_NearFarClip.y - u_NearFarClip.x),
                 0,
                 1.0f);
}

void main() {
    vec3 position = getViewPos(fs_UV).xyz;
    vec3 norm  = texture(u_ViewNormalMap, fs_UV).xyz;
    float maxDepth = 0;
    vec2 dimension = textureSize(u_AlbedoMap, 0).xy;


    vec3 samplePosition  = vec3(0.0);
    vec3 maxDistPosition = vec3(0);

    for (int i = -OUTLINE_WIDTH; i <= OUTLINE_WIDTH; ++i) {
        for (int j = -OUTLINE_WIDTH; j <= OUTLINE_WIDTH; ++j) {

            vec2 sampleUv = fs_UV + (vec2(i, j) * 1.) / dimension;

#if defined UseNormCull
            vec3 sampleNorm = texture(u_ViewNormalMap, sampleUv).xyz;
            if(length(sampleNorm - norm) < 0.001f){
                continue;
            }
#endif
            samplePosition = getViewPos(fs_UV + (vec2(i, j) * 1.) / dimension).xyz;
            if(abs(position.z - samplePosition.z) > maxDepth){
                maxDepth = abs(position.z - samplePosition.z);
            }
        }
    }


// The intensity is depends on both depth term
//  1. the depth difference in view space
//  2. the actual depth of the position in screen space
//        further position should have a thinner outline
    float depthFactor = 1 - smoothstep(OUTLINE_DEPTH_FACTOR.x,
                                   OUTLINE_DEPTH_FACTOR.y,
                                   getDepth(position.z));

    float intensity =   smoothstep(OUTLINE_INTENSITY_FACTOR.x,
                                   OUTLINE_INTENSITY_FACTOR.y,
                                   maxDepth);
    intensity *= depthFactor;

    color = texture(u_AlbedoMap, fs_UV);
    vec3 lineColor = darkenColor(color.rgb);

    //color.rgb = mix(vec3(1), lineColor, intensity);
    //return;
    color.rgb = mix(color.rgb, lineColor, intensity);
}
