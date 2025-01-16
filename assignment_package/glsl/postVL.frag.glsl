#version 330

in vec2 fs_UV;

out vec4 color;

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_PositionMap;
uniform int u_Time;

uniform bool u_EnableShader;

const float PI = 3.14159265359;
const float invPI = 1.0 / PI;
const vec3 nightColor = vec3(0.1, 0.1, 0.2);
const vec3 moonColor = vec3(0.5, 0.5, 0.8);

vec3 sunDir;
float sunFactor;
float dayTime;

const vec3 sunBand[5] = vec3[]
(
    vec3(0.741, 0.408, 0.408),
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

float squareCurve(float x)
{
    return min(smoothstep(-0.1, 0.1, x), 1. - smoothstep(0.9, 1.1, x));
}

float densityCurve(float x)
{
    return 1.f;
}

float fog(float x)
{
    float factor = max(smoothstep(0.5f, 0.9f, 1.0f - x),
                       smoothstep(0.5f, 0.9f, x));
    return 0.7f * factor + 0.3f;
}

void main()
{
    vec4 baseCol = texture(u_AlbedoMap, fs_UV);
    if(!u_EnableShader){
        color = baseCol;
        return;
    }

    float angle = mod(u_Time * 0.0005, 2.0 * PI);
    dayTime = angle * invPI;
    sunDir = vec3(0, sin(angle), cos(angle));
    sunFactor = (1. - step(1., dayTime)) * squareCurve(dayTime)
            + step(1., dayTime) * (1. - squareCurve(dayTime - 1.));

    vec3 sunCol = getSunColor();
    float density = texture(u_PositionMap, fs_UV).r;
    float factor = 1.3f * fog(fract(dayTime));
    color = baseCol + factor * density * vec4(sunCol, 0.f);
}
