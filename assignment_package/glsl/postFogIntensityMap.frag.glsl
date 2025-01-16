#version 330
// Fog Effect
// Reference:
//  https://iquilezles.org/articles/fog/
in vec2 fs_UV;

out float intensity;

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_PositionMap;
uniform int u_Time;


uniform mat4 u_View;
uniform vec2 u_NearFarClip;
uniform vec3 u_CameraPos;

uniform bool u_EnableShaderFeature1;    //ss_EnablePostDistFog
uniform bool u_EnableShaderFeature2;    //ss_EnablePostHeightFog


const float FOG_DIENSITY = 5.0f;
const float HEIGHT_FOG_DIENSITY = 6.0f;

/// The distance range that fog will start work
/// In percentage of far clip, e.g. 0.0 means fog will act on all depth
/// The fog dist will be maximum at the medium of the day,
/// and will be minimum at the beginning/end of the day
const vec2  FOG_DIST_RANGE = vec2(0, 0.5f);


/// Dist when height fog shart work
const float HEIGHT_FOG_MIN_DIST = 0.15f;

const float PI = 3.14159265359;
const float invPI = 1.0 / PI;

/// Current daytime
/// [0 - 1] is day
/// [1 - 2] is night
float dayTime;
float fogStartDist = 0.;

void calcDayTime(){
    float angle = mod(u_Time * 0.0005, 2.0 * PI);
    dayTime = angle * invPI;
}

void calcFogStartDist(){
    float frac = dayTime - int(dayTime);
    if(frac > 0.5f){
        frac = 1 - frac;
    }

    /// Increase the fog at the beginning and the end of the daytime
    float dayInterleaveTime = 0.25f;

    //  Adjustment in sun rise / sun set
    fogStartDist = FOG_DIST_RANGE.x +
            (FOG_DIST_RANGE.y - FOG_DIST_RANGE.x) * smoothstep(0, dayInterleaveTime, frac);
}

vec4 getViewPos(vec2 uv){
    return u_View * texture(u_PositionMap, uv);
}


float getDepth(float z){
    float nearClip = max(u_NearFarClip.x,
                         (u_NearFarClip.y - 0.01f) * fogStartDist);

    /// Note that z is negative in view space
    return clamp((abs(z) - nearClip) / (u_NearFarClip.y - nearClip),
                 0,
                 1.0f);
}

float heightFogIntensity(vec3 rgb, float cameraHeight, float targetHeight){
    if(targetHeight > cameraHeight){
        return 0;
    }

    float nearClip = max(u_NearFarClip.x,
                         (u_NearFarClip.y - 0.01f) * HEIGHT_FOG_MIN_DIST);

    float heightDiff = cameraHeight - targetHeight;
    targetHeight = clamp((abs(heightDiff) - nearClip) / (u_NearFarClip.y - nearClip),
                     0,
                     1.0f);

    return 1.0 - exp( - targetHeight * HEIGHT_FOG_DIENSITY );
}

float depthFogIntensity(vec3  rgb,       // original color of the pixel
                        float distance ) // camera to point distance
{
    float intensity = exp( -distance * FOG_DIENSITY );
    return 1.0 - smoothstep(0, 1, intensity);
}

float calculateLevel(float val, float levelCount){
    float lower = floor(val * levelCount) / levelCount;
    float lowerDist = val - lower;

    float upper = ceil(val * levelCount) / levelCount;
    float upperDist = upper - val;

    return upperDist > lowerDist ? upper : lower;
}

float calculateFogIntensity(vec4 targetPosition){
    if(!u_EnableShaderFeature1 && !u_EnableShaderFeature2){
        return 0;
    }


    float targetHeight = targetPosition.y;
    float targetDepth = targetPosition.z;
    if(targetPosition.w <= 0){
        targetHeight = u_CameraPos.y;
        targetDepth = u_NearFarClip.y;
    }
    else{
        targetDepth = getDepth((u_View * targetPosition).z);
    }


    if(texture(u_PositionMap, fs_UV).w <= 0){
        return 0;
    }

    vec4 color = texture(u_AlbedoMap, fs_UV);
    if(color.a <= 0){
        return 0;
    }

    float heightFog = u_EnableShaderFeature2 ?
                heightFogIntensity(
                color.rgb,
                u_CameraPos.y,
                targetHeight)
              :
                0;

    float distFog = u_EnableShaderFeature1 ?
                depthFogIntensity(color.rgb, targetDepth)
              :
                0;

    return mix(heightFog, 1, distFog);
}

void main() {
    calcDayTime();
    calcFogStartDist();

    intensity = 0;
    vec4 targetPosition = texture(u_PositionMap, fs_UV);
    intensity = calculateFogIntensity(targetPosition);
}
