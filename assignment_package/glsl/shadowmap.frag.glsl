#version 330

uniform float u_DepthBound[4];

uniform mat4 u_LightViewProj[3];

uniform vec3 u_LightDir;

uniform sampler2DArray u_LightDepthMap;

// this is albedeo actually
uniform sampler2D u_PosMap;

in vec4 fs_Pos;

in vec4 fs_Nor;

in vec2 fs_UV;

in vec4 fs_LightPos[3];

out vec4 out_Col;

const float PI = 3.14159265359;
const int PCF_SAMPLES = 9;
vec2 texelSize;

// hash frunction from
float hash12(const in vec2 p) {
    vec3 p3  = fract(vec3(p.xyx) * 0.1031f);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

float hash13(const in vec3 p) {
    vec3 p3 = fract(p * 0.1031);
    p3 += dot(p3, p3.zyx + 31.32);
    return fract((p3.x + p3.y) * p3.z);
}

float LinearizeDepth(float depth)
{
    float near = u_DepthBound[0];
    float far = u_DepthBound[3];
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

float getBlockerDistance(const in vec3 lightCoord, float bias, int layer)
{
    float blockerDists = 0.f;
    int blockers = 0;

    // rotation settings
    float angle = hash12(gl_FragCoord.xy) * (2.f * PI);
    vec2 rot = vec2(cos(angle), sin(angle));
    float angleStep = PI * 2.f / PCF_SAMPLES;
    vec2 rotStep = vec2(cos(angleStep), sin(angleStep));
    mat2 rotMat = mat2(rotStep, -rotStep.y, rotStep.x);

    for (int i = 0; i < PCF_SAMPLES; ++i)
    {
        rot = rotMat * rot;
        float n = hash13(vec3(gl_FragCoord.xy, i + 1024));
        vec2 offset = rot * n * texelSize.x;
        float pcssDepth = texture(u_LightDepthMap, vec3(lightCoord.xy + offset, layer)).r;
        if (pcssDepth < lightCoord.z - bias)
        {
            ++blockers;
            blockerDists += pcssDepth;
        }
    }

    if (blockers > 0)
    {
        return blockerDists / float(blockers);
    }
    else
    {
        return -1.f;
    }
}

float getPCFKernelSize(const in vec3 lightCoord, float bias, int layer)
{
    float blockerDists = getBlockerDistance(lightCoord, bias, layer);

    if (blockerDists == -1.f)
        return 1.f;

    float penumbraWidth = lightCoord.z - bias - blockerDists;
    return penumbraWidth;
    return penumbraWidth * u_DepthBound[0] / blockerDists;
}

void main()
{
    vec4 baseCol = texture(u_PosMap, fs_UV);
    if (baseCol.a == 0.f) discard;

    float invW = gl_FragCoord.w;
    float depth = LinearizeDepth(gl_FragCoord.z);
    vec3 normal = normalize(vec3(fs_Nor));
    float visibility = clamp(dot(normal, u_LightDir), 0.f, 1.f);
    float bias = clamp(0.001f * sqrt(1.f - visibility * visibility) / visibility, 0.0001f, 0.001f);

    int layer = 3;
    for (int i = 0; i < 3; ++i)
    {
        //vec2 lightCoord = vec2(fs_LightPos[i].x / fs_LightPos[i].w, fs_LightPos[i].y / fs_LightPos[i].w);
        //lightCoord = 0.5 * lightCoord + 0.5;
        if (depth < u_DepthBound[i + 1])
        {
            layer = i;
            break;
        }
    }

    if (layer < 3)
    {
        vec3 lightCoord = fs_LightPos[layer].xyz / fs_LightPos[layer].w;
        lightCoord = 0.5f * lightCoord + 0.5f;

        if (lightCoord.z > 1.)
        {
            out_Col = vec4(0.0f);
            return;
        }
        //bias *= 1.f / (u_DepthBound[layer + 1] * 0.05f);
        float biasMul[3] = float[3](1.f, 2.f, 5.f);
        bias *= biasMul[layer];
        //bias = 0.f;

        float shadow = 0.0f;
        texelSize = 1.0 / vec2(textureSize(u_LightDepthMap, 0));
        //float radius = getPCFKernelSize(lightCoord, bias, layer);
        //radius = min(1.f, radius * 50.f);

        // rotation settings
        float angle = hash12(gl_FragCoord.xy) * (2.f * PI);
        vec2 rot = vec2(cos(angle), sin(angle));
        float angleStep = PI * 2.f / PCF_SAMPLES;
        vec2 rotStep = vec2(cos(angleStep), sin(angleStep));
        mat2 rotMat = mat2(rotStep, -rotStep.y, rotStep.x);

        for (int i = 0; i < PCF_SAMPLES; ++i)
        {
            rot = rotMat * rot;
            float n = 3.0f * hash13(vec3(gl_FragCoord.xy, i));
            vec2 offset = rot * n * texelSize.x;
            float pcfDepth = texture(u_LightDepthMap, vec3(lightCoord.xy + offset, layer)).r;
            float diff = lightCoord.z - bias - pcfDepth;
            shadow += (lightCoord.z - bias) > pcfDepth ? 0.1f : 1.0f;
        }

        shadow /= PCF_SAMPLES;

        //shadow = visibility > 0.01f ? shadow : 0.0f;
        //float pcfDepth = texture(u_LightDepthMap, vec3(lightCoord.xy, layer)).r;

        out_Col = vec4(shadow, 0, 0, 1);
    }
    else
    {
        out_Col = vec4(0.0f);
        return;
    }
}
