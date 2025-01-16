#version 330
// ^ Change this to version 130 if you have compatibility issues

// This is a fragment shader. If you've opened this file first, please
// open and read lambert.vert.glsl before reading on.
// Unlike the vertex shader, the fragment shader actually does compute
// the shading of geometry. For every pixel in your program's output
// screen, the fragment shader is run for every bit of geometry that
// particular pixel overlaps. By implicitly interpolating the position
// data passed into the fragment shader by the vertex shader, the fragment shader
// can compute what color to apply to its pixel based on things like vertex
// position, light position, and vertex color.
uniform int u_Time;
uniform sampler2D u_AlbedoMap;
uniform sampler2D u_NormalMap;
uniform vec4 u_Color; // The color with which to render this instance of geometry.
// These are the interpolated values out of the rasterizer, so you can't know
// their specific values without knowing the vertices that contributed to them
in vec4 fs_Pos;
in vec4 fs_Nor;
in vec4 fs_LightVec;
in vec2 fs_UV;
flat in int fs_Type;
in mat3 TBN;
in vec2 fs_Velocity;
// This is the final output color that you will see on your
// screen for the pixel that is currently being processed.
layout (location = 0) out vec4 out_Col;


const float PI = 3.14159265359;
const float invPI = 1.0 / PI;
const vec3 skyColor = vec3(1.0);
const vec3 nightColor = vec3(0.1, 0.1, 0.2);
const vec3 moonColor = vec3(0.5, 0.5, 0.8);

vec3 sunDir;
float sunFactor;
float dayTime;

vec3 getSkyAbsorption(vec3 x, float y)
{
    vec3 absorption = x * -y;
    absorption = exp2(absorption) * 3.6;
    return absorption;
}

float zenithDensity(float x)
{
    return 0.7 / pow(max(x, 0.35e-2), 0.75);
}

vec3 reinhardJodieTonemap(vec3 c)
{
    float illum = dot(c, vec3(0.2126, 0.7152, 0.0722));
    vec3 tc = c / (c + 1.0);
    return mix(c / (illum + 1.0), tc, tc);
}

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

float random1(vec3 p) {
    return fract(sin(dot(p,vec3(127.1, 311.7, 191.999)))
                 *43758.5453);
}

float mySmoothStep(float a, float b, float t) {
    t = smoothstep(0, 1, t);
    return mix(a, b, t);
}

float cubicTriMix(vec3 p) {
    vec3 pFract = fract(p);
    float llb = random1(floor(p) + vec3(0,0,0));
    float lrb = random1(floor(p) + vec3(1,0,0));
    float ulb = random1(floor(p) + vec3(0,1,0));
    float urb = random1(floor(p) + vec3(1,1,0));

    float llf = random1(floor(p) + vec3(0,0,1));
    float lrf = random1(floor(p) + vec3(1,0,1));
    float ulf = random1(floor(p) + vec3(0,1,1));
    float urf = random1(floor(p) + vec3(1,1,1));

    float mixLoBack = mySmoothStep(llb, lrb, pFract.x);
    float mixHiBack = mySmoothStep(ulb, urb, pFract.x);
    float mixLoFront = mySmoothStep(llf, lrf, pFract.x);
    float mixHiFront = mySmoothStep(ulf, urf, pFract.x);

    float mixLo = mySmoothStep(mixLoBack, mixLoFront, pFract.z);
    float mixHi = mySmoothStep(mixHiBack, mixHiFront, pFract.z);

    return mySmoothStep(mixLo, mixHi, pFract.y);
}

float fbm(vec3 p) {
    float amp = 0.5;
    float freq = 4.0;
    float sum = 0.0;
    for(int i = 0; i < 8; i++) {
        sum += cubicTriMix(p * freq) * amp;
        amp *= 0.5;
        freq *= 2.0;
    }
    return sum;
}

float squareCurve(float x)
{
    return min(smoothstep(-0.1, 0.1, x), 1. - smoothstep(0.9, 1.1, x));
}

void main()
{
    // Material base color (before shading)
        vec4 baseCol = texture(u_AlbedoMap, fs_UV);
        if (fs_Type == 8)
        {
            out_Col = baseCol;
            return;
        }

        //diffuseColor = diffuseColor * (0.5 * fbm(fs_Pos.xyz) + 0.5);
        vec3 normal = texture(u_NormalMap, fs_UV).rgb;
        normal = normal * 2.0 - 1.0;
        normal = TBN * normal;
        normal = normalize(normal);

        float angle = mod(u_Time * 0.0005, 2.0 * PI);
        dayTime = angle * invPI;
        sunDir = vec3(0, sin(angle), cos(angle));
        sunFactor = (1. - step(1., dayTime)) * squareCurve(dayTime)
                + step(1., dayTime) * (1. - squareCurve(dayTime - 1.));

        // Calculate the diffuse term for Lambert shading
        float diffuseTerm = dot(normal, sunDir);
        diffuseTerm = mix(dot(normal, -sunDir), diffuseTerm, sunFactor);
        // Avoid negative lighting values
        diffuseTerm = clamp(diffuseTerm, 0, 1);

        float ambientTerm = 0.5;

        vec3 sunCol = 0.5 * getSunColor() * diffuseTerm;
        vec3 ambient = mix(nightColor, skyColor, sunFactor) * ambientTerm;

        vec3 lightIntensity = sunCol + ambient;   //Add a small float value to the color multiplier
                                                            //to simulate ambient lighting. This ensures that faces that are not
                                                            //lit by our point light are not completely black.

        // Compute final shaded color
        out_Col = vec4(1.2 * lightIntensity * baseCol.rgb, baseCol.a);
}
