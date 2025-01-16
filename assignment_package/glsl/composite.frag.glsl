#version 330

uniform int u_Time;
uniform vec3 u_LightDir;
uniform mat4 u_View;

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_PosMap;
uniform sampler2D u_ShadowMap;
uniform sampler2D u_SSAOMap;
uniform sampler2D u_SkyMap;


uniform bool u_EnableShaderFeature1;    // ss_EnablePassShadow
uniform bool u_EnableShaderFeature2;    // ss_EnableDayNightAmbient
uniform bool u_EnableShaderFeature3;    // ss_EnablePostDistFog


in vec2 fs_UV;

out vec4 out_Col;


const float PI = 3.14159265359;
const float invPI = 1.0 / PI;
const vec3 skyColor = vec3(0.8, 0.9, 1.0);
const vec3 nightColor = vec3(0.1, 0.1, 0.2);
const vec3 moonColor = vec3(0.5, 0.5, 0.8);

vec3 sunDir;
float sunFactor;
float dayTime;

const vec3 sunBand[5] = vec3[]
(
    vec3(0.741, 0.508, 0.508),
    vec3(0.914, 0.733, 0.576),
    vec3(1.0),
    vec3(0.914, 0.733, 0.576),
    vec3(0.741, 0.408, 0.408)
);

vec3 getSunColor()
{
    float angle = mod(u_Time * 0.0005, 2.0 * PI) / PI;
    float sunFactor = min(smoothstep(-0.1, 0.1, angle), 1. - smoothstep(0.9, 1.1, angle));
    vec3 sunColor;
    if (angle < 0.2) sunColor = mix(sunBand[0],sunBand[1], angle / 0.2);
    else if (angle < 0.5) sunColor = mix(sunBand[1],sunBand[2], (angle - 0.2) / 0.3);
    else if (angle < 0.8) sunColor = mix(sunBand[2],sunBand[3], (angle - 0.5) / 0.3);
    else sunColor = mix(sunBand[3],sunBand[4], (angle - 0.8) / 0.2);

    return mix(moonColor, sunColor, sunFactor);
}

float random1(vec3 p) {
    return fract(sin(dot(p,vec3(127.1, 311.7, 191.999)))
                 *43758.5453);
}

vec3 reinhardJodieTonemap(vec3 c)
{
    float illum = dot(c, vec3(0.2126, 0.7152, 0.0722));
    vec3 tc = c / (c + 1.0);
    return mix(c / (illum + 1.0), tc, tc);
}

float squareCurve(float x)
{
    return min(smoothstep(-0.1, 0.1, x), 1. - smoothstep(0.9, 1.1, x));
}

float getFogFactor(float depth)
{
    const float fogMax = 350.f;
    const float fogMin = 10.f;

    float factor = (fogMax - depth) / (fogMax - fogMin);
    factor = clamp(2.f * factor, 0.f, 1.f);
    return pow(factor, 3.f);
}

void main()
{
    vec4 baseCol = texture(u_AlbedoMap, fs_UV);
    if (baseCol.a <= 0.f){
        out_Col = vec4(0);
        return;
    }

    vec3 normal = texture(u_NormalMap, fs_UV).rgb;
    normal = normal * 2.0 - 1.0;
    normal = normalize(normal);

    vec4 pos = texture(u_PosMap, fs_UV);
    pos = u_View * pos;
    float depth = -pos.z;
    float fogFactor = getFogFactor(depth);

    float angle = mod(u_Time * 0.0005, 2.0 * PI);
    dayTime = angle * invPI;
    sunDir = vec3(0, sin(angle), cos(angle));
    sunFactor = (1. - step(1., dayTime)) * squareCurve(dayTime)
            + step(1., dayTime) * (1. - squareCurve(dayTime - 1.));

    vec3 sunCol = getSunColor();

    // Calculate the diffuse term for Lambert shading
    float diffuseTerm = dot(normal, u_LightDir);
    diffuseTerm = clamp(diffuseTerm, 0.f, 1.f);

    float ambientTerm = 0.5f * squareCurve(fract(dayTime));
    float ssaoTerm = texture(u_SSAOMap, fs_UV).x;
    ambientTerm *= ssaoTerm;

    vec3 diffuse = 1.f * sunCol * diffuseTerm;
    vec3 ambient = vec3(ambientTerm);
    if(u_EnableShaderFeature2){
        ambient *= mix(nightColor, skyColor, sunFactor);
    }

    float shadowTerm = u_EnableShaderFeature1 ?
                clamp(texture(u_ShadowMap, fs_UV).r, 0.0f, 1.f) : 0;

    vec3 color = (shadowTerm * diffuse + ambient) * baseCol.rgb;

    if(u_EnableShaderFeature3){
        vec3 fogColor = texture(u_SkyMap, fs_UV).rgb;
        color = mix(fogColor, color, fogFactor);
    }

    //color += texture(u_VolumeLightMap, fs_UV).r * sunCol;


    // Compute final shaded color
    out_Col = vec4(clamp(color, 0.f, 1.2f), baseCol.a);
}
