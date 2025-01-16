#version 330
// Hatching
// Referemce: https://www.shadertoy.com/view/MsSGD1

uniform ivec2 u_Dimensions;
uniform int u_Time;

in vec2 fs_UV;

out vec4 color;

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_PositionMap;

uniform mat4 u_View;
uniform vec2 u_NearFarClip;

const int FREQ = 15;
const float HATCH_WIDTH_INV = 300f; // The larger it is, the thinner the hatch
const float DARKEN_THRESHOLD = 0.3f;


float random2( float p ) {
    return fract(sin(p) * 43758.5453);
}


float luminosity(vec2 uv){
    return dot(texture2D(u_AlbedoMap, uv).rgb, vec3(0.21, 0.72, 0.07));
}

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


    float lum = luminosity(fs_UV);

    color = texture2D(u_AlbedoMap, fs_UV);
    /// Only draw dark hatch line when the luminosity is low
    if(lum > DARKEN_THRESHOLD){
        return;
    }

    /// Variying the hatchs based on the depth
    float depth = getDepth(getViewPos(fs_UV).z);
    float depthFactor = smoothstep(.2f, .8f, depth) + 1;
    float hatchWidth = HATCH_WIDTH_INV * depthFactor;
    float hatchFreq = FREQ / depthFactor;

    /// Shift the hatch over time
    float hatch = sin((fs_UV.x + fs_UV.y) * hatchWidth * 3.14159 +
                      u_Time / hatchFreq *
                      (random2(u_Time / hatchFreq) - 0.5f) * 2);

    if(hatch > 0.1){
        return;
    }
    //color.rgb = vec3(hatch, hatch, hatch);
    //return;
    float intensity = (1 - smoothstep(0, DARKEN_THRESHOLD * 1.3f, lum));
    color.rgb = mix(color.rgb, darkenColor(color.rgb), intensity);
}
