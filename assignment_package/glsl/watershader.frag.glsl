#version 330

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_PosMap;
uniform sampler2D u_SkyMap;

uniform mat4 u_View;
uniform mat4 u_Proj;
uniform mat4 u_ViewProj;
uniform mat3 u_ViewInv;

uniform vec3 u_LightDir;
uniform vec3 u_CameraPos;
uniform int u_Time;

uniform bool u_EnableShader;
uniform bool u_EnableShaderFeature1;    // ss_EnablePassWaterFresnel
uniform bool u_EnableShaderFeature2;    // ss_EnablePassWaterReflection


in vec4 fs_Pos;
in vec4 fs_Nor;
in mat3 TBN;

out vec4 out_Col;

const vec3 waterColor = vec3(0.188f, 0.339f, 0.608f);


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

const float DRAG_MULT = 0.1f;

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
    result /= 22.f * aSum;
    nor /= aSum;
    vec3 dx = vec3(1, nor.x, 0);
    vec3 dz = vec3(0, nor.y, 1);
    vec3 normal = normalize(vec3(nor.x, 1.f, nor.y));
    //vec3 normal = normalize(cross(dz, dx));
    return vec4(result, normal);
}

vec3 getNormal(vec2 pos)
{
    float e = 0.001f;
    vec2 ex = vec2(e, 0);
    float H = coarseWater(pos).x;
    vec3 a = vec3(pos.x, H, pos.y);
    return normalize(
    cross(
        a - vec3(pos.x - e, coarseWater(pos.xy - ex.xy).x, pos.y),
        a - vec3(pos.x, coarseWater(pos.xy - ex.yx).x, pos.y + e)
    )
    );
}

const float rayStep = 1.0f;
const int maxSteps = 200;

vec2 binarySearch(vec3 rd, vec3 wpos)
{
    vec4 lpos = vec4(wpos - rd * rayStep, 1.f);
    vec4 rpos = vec4(wpos, 1.f);
    vec4 mpos;
    vec4 vpos = vec4(0);
    vec4 ndc = vec4(0);
    vec4 mapPos = vec4(0);

    for (int i = 0; i < 8; ++i)
    {
        mpos = (lpos + rpos) * 0.5f;
        vpos = u_View * mpos;
        ndc = u_Proj * vpos;
        ndc /= ndc.w;
        ndc = ndc * 0.5f + 0.5f;

        mapPos = u_View * texture(u_PosMap, ndc.xy);
        float dDepth = vpos.z - mapPos.z;

        if (abs(dDepth) < 0.05f)
        {
            return ndc.xy;
        }
        else if (dDepth < 0.f)
        {
            rpos = mpos;
        }
        else
        {
            lpos = mpos;
        }
    }
    return ndc.xy;
}

vec3 sampleSky(vec3 ro, vec3 rd)
{
    vec3 rov = (u_View * vec4(ro, 1.f)).xyz;
    vec3 rdv = mat3(u_View) * rd;
    if (rdv.z > 0) rdv.z  = -rdv.z;

    //ro + t*rd = (x, y, -250)
    float t = (-250.f - rov.z) / rdv.z;
    vec4 p = u_Proj * vec4(rov + t * rdv, 1.f);
    p /= p.w;
    p = 0.5f * p + 0.5f;
    return texture(u_SkyMap, p.xy).rgb;
    if (clamp(p, 0.f, 1.f) == p)
    {
        return texture(u_SkyMap, p.xy).rgb;
    }
    else
    {
        return skyColor;
    }
}


vec3 getReflection(vec3 ro, vec3 rd)
{
    vec3 wpos = ro + rd * rayStep;
    vec4 vpos = vec4(0);
    vec4 ndc = vec4(0);
    vec4 mapPos = vec4(0);

    for (int i = 0; i < maxSteps; ++i)
    {
        //project p to NDC space
        vpos = u_View * vec4(wpos, 1.f);
        ndc = u_Proj * vpos;
        ndc /= ndc.w;
        ndc = ndc * 0.5f + 0.5f;

        // ray traced outside of frustum
        if (clamp(ndc, 0.f, 1.f) != ndc)
        {
            //return (ndc - clamp(ndc, 0.f, 1.f)).xyz;
            return sampleSky(ro, rd);
        }

        // map position in view space
        mapPos = u_View * texture(u_PosMap, ndc.xy);

        // point is on sky, continue
        if (mapPos.a == 0.f) continue;

        float dDepth = vpos.z - mapPos.z;

        //if (rd.z - dDepth < 1.2f)
        if (dDepth <= 0.f)
        {
            vec2 coord = binarySearch(rd, wpos);
            return texture(u_AlbedoMap, coord).rgb;
        }

        wpos += rd * rayStep;
    }
    return sampleSky(ro, rd);
    return texture(u_SkyMap, ndc.xy).rgb;
}

void main()
{
    if(!u_EnableShader){
        discard;
    }

    float angle = mod(u_Time * 0.0005, 2.0 * PI);
    dayTime = angle * invPI;
    sunDir = vec3(0, sin(angle), cos(angle));
    sunFactor = (1. - step(1., dayTime)) * squareCurve(dayTime)
            + step(1., dayTime) * (1. - squareCurve(dayTime - 1.));

    vec3 sunCol = getSunColor();

    vec3 normal = normalize(fs_Nor).rgb;
    if (normal.y > 0)
    {
        //normal = coarseWater(0.2f * fs_Pos.xz).yzw;
        normal = getNormal(0.8f * fs_Pos.xz);
    }


    vec3 viewDir = normalize(u_CameraPos - fs_Pos.xyz);
    vec3 lightDir = u_LightDir;
    vec3 halfWay = normalize(viewDir + lightDir);
    float specularTerm = pow(clamp(dot(halfWay, normal), 0, 1), 4);

    vec3 rd = normalize(reflect(-viewDir, normal));

    vec3 reflection =   u_EnableShaderFeature2 ?
                        getReflection(fs_Pos.xyz, rd) : vec3(0);

    float base = 1.f - clamp(dot(viewDir, normal), 0, 1);
    float exponential = pow(base, 5.0f);
    float fresnel = u_EnableShaderFeature1 ?
                    (exponential + 0.04f * (1.0f - exponential)) : 0.;
    //fresnel *= 0.9f;

    //specularTerm *= fresnel;

    // Calculate the diffuse term for Lambert shading
    float diffuseTerm = dot(normal, u_LightDir);
    diffuseTerm = 0.1f * clamp(diffuseTerm, 0.f, 1.f);

    float ambientTerm = 0.4f * squareCurve(fract(dayTime));

    vec3 diffuse = 1.f * sunCol * diffuseTerm;
    vec3 ambient = mix(nightColor, skyColor, sunFactor) * ambientTerm;

    vec3 color = (1.f - fresnel) * ambientTerm * waterColor + fresnel * (specularTerm * sunCol + reflection);

    float alpha = 0.5f;
    vec4 bedPos = texelFetch(u_PosMap, ivec2(gl_FragCoord.xy), 0);
    if (bedPos.a > 0.f)
    {
        float dist = length(fs_Pos - bedPos);
        alpha = 0.65f * smoothstep(2.f, 15.f, dist) + 0.35f;
        alpha = (1.f - fresnel) * alpha + fresnel;
    }
    else
    {
        alpha = 1.f;
        alpha = (1.f - fresnel) * alpha + fresnel;
        vec3 skyCol = texelFetch(u_SkyMap, ivec2(gl_FragCoord.xy), 0).rgb;
        color = mix(color, skyCol, alpha);
    }


    out_Col = vec4(color, alpha);
}
