#version 330

uniform mat4 u_Model;

uniform mat4 u_ViewProj;

uniform int u_Time;

in vec4 vs_Pos;

in vec4 vs_Nor;

out vec4 fs_Pos;
out vec4 fs_Nor;
out mat3 TBN;

void setTBN()
{
    vec3 N = vs_Nor.rgb;
    vec3 T = normalize(cross(N, vec3(0, 1, 1)));
    vec3 B = cross(N, T);
    TBN = mat3(T,N,B);
}

const float DRAG_MULT = 0.3f;

float hash( float n )
{
    return fract(sin(n)*43758.5453);
}

// get wave value and derivative given position and wave direaction
vec2 getWave(vec2 pos, vec2 dir, float freq, float speed)
{
  // project pos to dir
  float x = dot(dir, pos) * freq + speed * u_Time;;
  float wave = exp(sin(x) - 1.0);
  // derivative of e^sin(x) = cos(x) * e^sin(x)
  float dd = freq * wave * cos(x);
  return vec2(wave, -dd);
}

vec4 coarseWater(vec2 pos)
{
    float amplitude = 1.0f;
    float freq = 1.0f;
    float angle = 0.f;
    float aSum = 0.f;
    int iters = 6;
    float speed = 0.006f;

    float result = 0.f;
    vec2 nor = vec2(0);

    for (int i = 0; i < iters; ++i)
    {
        // generate random directinos
        vec2 dir = vec2(cos(angle), sin(angle));
        angle += 7253.399963f;

        vec2 wave = getWave(pos, dir, freq, speed);

        result += wave.x * amplitude;
        nor += dir * wave.y * amplitude;
        aSum += amplitude;

        pos += dir * wave.y * amplitude * DRAG_MULT;

        amplitude *= 0.82f;
        freq *= 1.28f;
        speed *= 1.07f;
    }
    result /= 15.f * aSum;
    nor /= aSum;
    vec3 dx = vec3(1, nor.x, 0);
    vec3 dz = vec3(0, nor.y, 1);
    vec3 normal = normalize(vec3(nor.x, 1.f, nor.y));
    //vec3 normal = normalize(cross(dz, dx));
    return vec4(result, normal);
}

void main()
{
    fs_Pos = vs_Pos;
    fs_Nor = vs_Nor;
    setTBN();

    if (vs_Nor.y > 0.)
    {
        fs_Pos.y -= 0.5f;
        float scale = 0.8f;
        vec4 wave = coarseWater(scale * fs_Pos.xz);
        fs_Pos.y += 0.5f * wave.r;
        vec3 normal = wave.yzw;
        vec3 tangent = normalize(vec3(1.f, normal.x, 0));
        vec3 bitangent = normalize(vec3(0, normal.z, 1.f));
        normal = normalize(cross(bitangent, tangent));
        TBN = mat3(tangent, normal, bitangent);
    }

    vec4 modelposition = u_Model * fs_Pos;

    gl_Position = u_ViewProj * modelposition;
}
