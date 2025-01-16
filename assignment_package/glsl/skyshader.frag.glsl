#version 330

uniform ivec2 u_Dimensions;
uniform int u_Time;
uniform vec3 u_CameraPos;
uniform mat4 u_View;
uniform mat3 u_ViewInv;

in vec2 fs_UV;

layout (location = 0) out vec4 out_Col;
layout (location = 1) out vec4 out_pure_sky;


/*
  =============================
  Atmospheric Sky
  =============================
*/

const float PI = 3.14159265359;
const float invPI = 1.0 / PI;
const vec3 skyColor = vec3(0.32, 0.5, 1.0);
const vec3 nightColor = vec3(0.1, 0.1, 0.2);
const vec3 moonColor = vec3(0.5, 0.5, 0.7);

vec3 sunDir;
float sunFactor;
float dayTime;

vec3 getAbsorption(vec3 x, float y)
{
    vec3 absorption = - x * y;
    absorption = exp2(absorption) * 3.2;
    return absorption;
}

float horizonCurve(float x)
{
    float y = clamp(x + 0.3 + sunDir.y * sunFactor * 0.2f, 0.01f, 1.3f);
    return min(0.5 / pow(y, 1.8), 4.);
}

// use smoothstep to prolong sun set / rise time
float sunCurve(float x)
{
    return 11.0 * (1.0 - smoothstep(0.0, 1.0, x)) + 1.0;
}

float getRayleigh(vec3 p)
{
    return 1.0 + pow(1.0 - clamp(distance(p, sunDir), 0.0, 1.0), 2.0) * PI * 0.5;
}

float getMie(vec3 p)
{
    float dist = clamp(1.0 - pow(distance(p, sunDir), 0.1), 0.0, 1.0);
    return pow(dist, 2.) * (3.0 - 2.0 * dist) * 2.0 * PI;
}

vec3 reinhardJodieTonemap(vec3 c)
{
    float illum = dot(c, vec3(0.2126, 0.7152, 0.0722));
    vec3 tc = c / (c + 1.0);
    return mix(c / (illum + 1.0), tc, tc);
}

// p is the ray intersect on the sphere of radius 1
vec3 getSkyColor(vec3 p){

    float horDensity = horizonCurve(p.y);
    float sunHeight =  clamp(sunDir.y, 0.0, 1.0);

    vec3 skyAbsorption = getAbsorption(skyColor, horDensity);
    vec3 sunAbsorption = getAbsorption(skyColor, sunCurve(sunDir.y));
    float rayleigh = getRayleigh(p);
    vec3 sky = skyColor * horDensity * rayleigh;
    vec3 mie = getMie(p) * sunAbsorption;
    float energy = length(sunAbsorption);

    vec3 totalSky = mix(sky * skyAbsorption, sky / (sky + 1.), sunHeight) + mie;
    totalSky *= (sunAbsorption + energy) * 1.6;
    return totalSky;
}


vec3 getSunColor()
{
    vec3 color = getAbsorption(skyColor, sunCurve(sunDir.y));
    color = 1.2 * reinhardJodieTonemap(color).xyz;
    return color * sunFactor;
}


/*
  =============================
  Procedural Cloud
  =============================
*/


// fbm noise from iq
float hash( float n )
{
    return fract(sin(n)*43758.5453);
}

mat3 m = mat3( 0.00,  0.80,  0.60,
              -0.80,  0.36, -0.48,
              -0.60, -0.48,  0.64 );

float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);

    f = f*f*(3.0-2.0*f);

    float n = p.x + p.y*57.0 + 113.0*p.z;

    float res = mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                        mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
                    mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                        mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
    return res;
}

float fbm( vec3 p )
{
    float f;
    f  = 0.5000*noise( p ); p = m*p*2.02;
    f += 0.2500*noise( p ); p = m*p*2.03;
    f += 0.1250*noise( p );
    return f;
}

// noise map
float map(vec3 p)
{
    float sum = 0.;
    float amp = 2.05;
    float fre = 2.1;
    for( int i=0; i<8; ++i )
    {
        sum += (2.1 * noise(p * fre) - 1.) * amp;
        amp *= 0.5;
        fre *= 2.;
    }
    return sum + 0.1;
}

const int CLOUD_CENTER = 512;
const int CLOUD_RAD = 160;

// cloud sdf
// return density and normal
vec4 cloudSDF(vec3 p)
{
    // distance density
    float d = abs(p.y - CLOUD_CENTER);
    // simple normal assumption
    vec3 nor = vec3(0.0, sign(p.y - CLOUD_CENTER), 0.0);

    float m = map(p * 0.002 + 0.0001 * u_Time * vec3(1., 0., 0.25));
    d += 200.0 * m * (0.8 + 0.2 * nor.y);

    return vec4(-d, nor);
}

vec4 cloud(vec3 ro, vec3 rd)
{
    vec4 col = vec4(0.0);

    float tmin = (352. - ro.y) / rd.y;
    float tmax = (672. - ro.y) / rd.y;
    if (tmin < 0.0 || tmin > 5000.) return col;
    tmax = min(tmax, 5000.);

    float t = tmin;
    for(int i = 0; i < 32; ++i)
    {
        vec3 pos = ro + t * rd;
        vec4 v = cloudSDF(pos);
        // map density to [0, 1], clamp at 0.5
        float den = min(v.x / 10.0, 0.5);
        // LOD sample. High step at distant points.
        float dt = min(0.01 * t, 12.);
        if(den > 0.001)
        {
            vec3 sunLi = vec3(0);
            vec3 moonLi = vec3(0);

            // Sun shading
            if (dayTime < 1.1)
            {
                // sample at sun direction
                vec4 lightV = cloudSDF(pos + sunDir * 30.0);
                vec3 nor = v.yzw;
                // shadow term. represend the shadow casted by other clouds
                float shadow = (1.0 - smoothstep(-50.0, 50.0, lightV.x)) * 1.2;
                // diffuse term. same as lambert
                float diffuse = clamp(0.4 + 0.6 * dot(nor,sunDir), 0.0, 1.0 );
                // occlusion term. controls the contrast between direct and indirect lighting
                float occ = 0.2 + 0.7 * max(1.0 - lightV.x / 30.0, 0.0) + 0.1 * (1.0 - den);
                sunLi += 3.0 * getSunColor() * diffuse * occ * shadow + 0.1;
                sunLi += 0.5 * getSunColor() * (0.5 - 0.5 * nor.y) * occ;
            }

            // Moon shading
            if (dayTime > 0.9)
            {
                // sample at moon direction
                vec4 lightV = cloudSDF(pos - sunDir * 30.0);
                vec3 nor = v.yzw;
                // shadow term. represend the shadow casted by other clouds
                float shadow = (1.0 - smoothstep(-50.0, 50.0, lightV.x)) * 1.2;
                // diffuse term. same as lambert
                float diffuse = clamp(0.4 + 0.6 * dot(nor,-sunDir), 0.0, 1.0 );
                // occlusion term. controls the contrast between direct and indirect lighting
                float occ = 0.2 + 0.7 * max(1.0 - lightV.x / 30.0, 0.0) + 0.1 * (1.0 - den);
                moonLi += 3.0 * moonColor * diffuse * occ * shadow + 0.1;
                moonLi += 0.5 * moonColor * (0.5 - 0.5 * nor.y) * occ;
            }
            vec3 li = mix(moonLi, sunLi, sunFactor);

            float transmittance = clamp(den / dt, 0.0, 1.0);
            li.rgb *= transmittance;
            col += vec4(li.xyz, transmittance) * (1.0 - col.a);
        }
        else
        {
            dt = abs(den) + 1.;
        }
        float offset = fract(52.9829189f * fract(0.06711056f*float(gl_FragCoord.x) + 0.00583715f*float(gl_FragCoord.y)));
        t += dt + offset * 0.2;
        //t += dt;
        if(col.a > 0.995 || t > tmax) break;
    }

    return clamp(col, 0.0, 1.0);
}

float squareCurve(float x)
{
    return min(smoothstep(-0.1, 0.1, x), 1. - smoothstep(0.9, 1.1, x));
}

void main()
{
    float angle = mod(u_Time * 0.0005, 2.0 * PI);
    dayTime = angle * invPI;
    sunDir = vec3(0, sin(angle), cos(angle));
    sunFactor = (1. - step(1., dayTime)) * squareCurve(dayTime)
            + step(1., dayTime) * (1. - squareCurve(dayTime - 1.));

    float asRatio = float(u_Dimensions.x) / float(u_Dimensions.y);
    vec2 uv = fs_UV;
    // NDC space
    uv = uv * 2.0 - 1.0;

    float fov = tan(radians(22.5));
    uv.x = uv.x * asRatio * fov;
    uv.y = uv.y * fov;
    vec3 rd = vec3(uv, 1.0);
    // to world space direction
    rd = u_ViewInv * rd;
    rd = normalize(rd);

    vec3 skyCol = getSkyColor(rd);
    skyCol = reinhardJodieTonemap(skyCol);
    skyCol = pow(skyCol, vec3(2.2));

    rd = normalize(rd);
    vec4 cloudCol = cloud(u_CameraPos, rd);

    // TODO: use color band
    vec4 sunCol = smoothstep(0.999, 0.9995, dot(rd, sunDir)) * 3.0 * vec4(getSunColor(), 1.);
    vec4 moonCol = smoothstep(0.999, 0.9995, dot(rd, -sunDir)) * 1.0 * vec4(moonColor, 1.);

    skyCol = mix(nightColor, skyCol, sunFactor);
    sunCol = mix(moonCol, sunCol, sunFactor);

    out_pure_sky = vec4(skyCol, 1.0);
    out_Col = out_pure_sky + cloudCol * cloudCol.a + sunCol * (1. - cloudCol.a);
    //out_Col = vec4((rd + 1.) / 2., 1);
}
