# version 330
uniform sampler2D u_AlbedoMap;
uniform sampler2D u_VelocityMap;
uniform mat4 u_Model;
uniform mat4 u_ViewProj;
uniform mat4 u_ViewProjLastFrame;

uniform bool u_EnableShader;

in vec2 fs_UV;
out vec4 color;

/// CONSTS
const int   SAMPLE_STEP =  10;          // Determine the sample it takes for blur
const float SAMPLE_RATE =  0.5f;        // Determine the range of sampling, [x] means sample to x * velocity


void main()
{
    vec4 baseColor = texture(u_AlbedoMap, fs_UV);
    color = baseColor;

    if(!u_EnableShader){
        return;
    }


    // Check velocity, do nothing if is 0
    vec2 direction = texture(u_VelocityMap, fs_UV).xy;
    if(length(direction) <= 0){
        return;
    }

    // Do Blur
    float separation =  SAMPLE_RATE / float(SAMPLE_STEP);
    direction.xy *= separation;
    vec2  incUv = fs_UV;
    vec2  decUv = fs_UV;
    for(int i = 0; i < SAMPLE_STEP; ++i){
        incUv += direction;
        decUv -= direction;

        baseColor += texture(u_AlbedoMap, incUv);
        baseColor += texture(u_AlbedoMap, decUv);
    }
    baseColor /= ((2 * SAMPLE_STEP) + 1);
    color = baseColor;
}
