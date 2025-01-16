#version 330

uniform float u_DepthBound[4];

uniform mat4 u_LightViewProj[3];

uniform vec3 u_LightDir;

uniform mat4 u_ViewProj;

uniform mat3 u_ViewInv;

uniform int u_Time;

uniform vec3 u_CameraPos;

uniform sampler2DArray u_LightDepthMap;

uniform sampler2D u_PosMap;

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
    vec3(0.741, 0.408, 0.408),
    vec3(0.914, 0.733, 0.576),
    vec3(0.914, 0.733, 0.576),
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

float InterleavedGradientNoise(vec2 pixel)
{
    pixel += (float(u_Time) * 5.588238f);
    return fract(52.9829189f * fract(0.06711056f*float(pixel.x) + 0.00583715f*float(pixel.y)));
}

const float SCATTER = 0.1f;
const int VL_STEPS = 64;

float computeMie(float lightDotView)
{
    float result = 1.0f - SCATTER * SCATTER;
    result /= (4.0f * PI * pow(1.0f + SCATTER * SCATTER
                - (2.0f * SCATTER) * lightDotView, 1.5f));
    return result;
}

float getShadowValue(vec3 pos)
{
    float depth = (pos - u_CameraPos).z;
    int layer = 3;
    for (int i = 0; i < 3; ++i)
    {
        if (depth < u_DepthBound[i + 1])
        {
            layer = i;
            break;
        }
    }

    vec4 lightCoord = u_LightViewProj[layer] * vec4(pos, 1.f);
    lightCoord /= lightCoord.w;
    lightCoord = 0.5f * lightCoord + 0.5f;

    float lightDepth = texture(u_LightDepthMap, vec3(lightCoord.xy, layer)).r;
    return lightDepth - lightCoord.z;
}

float computeVL(vec3 ro, vec3 end)
{
    vec3 rayVec = end - ro;

    float rayLength = length(rayVec);
    vec3 rd = rayVec / rayLength;

    float stepLength = min(rayLength, 60.f) / VL_STEPS;

    vec3 step = rd * stepLength;

    vec3 p = ro + step * InterleavedGradientNoise(gl_FragCoord.xy);

    float density = 0.f;

    for (int i = 0; i < VL_STEPS; ++i)
    {
        float shadowVal = getShadowValue(p);

        if (shadowVal > 0.f)
        {
            density += 3.f;
            //density += 1.f;
        }
        p += step;
    }
    density *= computeMie(dot(rd, u_LightDir));

    return density / VL_STEPS;
}



float LinearizeDepth(float depth)
{
    float near = u_DepthBound[0];
    float far = u_DepthBound[3];
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

float squareCurve(float x)
{
    return min(smoothstep(-0.1, 0.1, x), 1. - smoothstep(0.9, 1.1, x));
}

void main()
{
    float angle = mod(u_Time * 0.0005, 2.0 * PI);
    dayTime = angle * invPI;
    sunDir = u_LightDir;
    sunFactor = (1. - step(1., dayTime)) * squareCurve(dayTime)
            + step(1., dayTime) * (1. - squareCurve(dayTime - 1.));

    vec4 pos_w = texture(u_PosMap, fs_UV);
    vec4 pos = u_ViewProj * pos_w;
    pos /= pos.w;

    // ray to sky
    if (pos_w.a == 0.f)
    {
        vec2 dimensions = vec2(textureSize(u_LightDepthMap, 0));
        float asRatio = dimensions.x / dimensions.y;
        vec2 uv = fs_UV;
        uv = uv * 2.0 - 1.0;

        float fov = tan(radians(22.5));
        uv.x = uv.x * asRatio * fov;
        uv.y = uv.y * fov;
        vec3 dir = vec3(uv, 1.0);
        dir = u_ViewInv * dir;
        dir = normalize(dir);
        pos_w.xyz = u_CameraPos + 200.f * dir;
    }

    vec3 ro = u_CameraPos;
    vec3 rd = normalize(pos_w.xyz - u_CameraPos);

    float lightDensity = computeVL(ro, vec3(pos_w));

    vec2 dimensions = vec2(textureSize(u_LightDepthMap, 0));
    float asRatio = dimensions.x / dimensions.y;
    vec2 uv = fs_UV;
    // NDC space
    uv = uv * 2.0 - 1.0;

    float fov = tan(radians(22.5));
    uv.x = uv.x * asRatio * fov;
    uv.y = uv.y * fov;
    //vec3 rd = vec3(uv, 1.0);
    // to world space direction
    //rd = u_ViewInv * rd;
    //rd = normalize(rd);

    vec3 p = ro + rd;

    float l = length(pos_w - vec4(p, 1.0)) / 250.f;


    out_Col = vec4(lightDensity, 0, 0, 1);
}
