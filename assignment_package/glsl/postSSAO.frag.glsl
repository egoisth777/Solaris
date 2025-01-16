# version 330

const int  NUM_NOISE   = 16;
const int  NUM_SAMPLES = 64;
const float RADIUS              = 1;    /// Sampling hemi-sphere radius
const float MIN_OCCULUSION_DIST = 0.01; /// minimum z-difference to be occuluded

uniform sampler2D u_PositionMap;
uniform sampler2D u_AlbedoMap;
uniform sampler2D u_ViewNormalMap;
uniform mat4 u_View;
uniform mat4 u_Proj;

uniform vec3 u_SamplePoints[NUM_SAMPLES];
uniform vec2 u_SampleNoise[NUM_NOISE];

uniform bool u_EnableShader;

in vec2 fs_UV;
layout (location = 0) out vec4 color;

vec3 getNoise(){
    int  noiseSize = int(sqrt(NUM_NOISE));
    ivec2 noiseCoord = ivec2(int(gl_FragCoord.x - 0.5) % noiseSize,
                             int(gl_FragCoord.y - 0.5) % noiseSize);
    int noiseId = noiseCoord.x + (noiseCoord.y * noiseSize);
    return normalize(vec3(u_SampleNoise[noiseId], 0));
}

vec4 getViewPos(vec2 uv){
    return u_View * texture(u_PositionMap, uv);
}

void main()
{
    if(!u_EnableShader){
        color = vec4(1);
        return;
    }

    color = texture(u_AlbedoMap, fs_UV);
    vec4 position = getViewPos(fs_UV);
    vec3 normal   = normalize(texture(u_ViewNormalMap,fs_UV).xyz);
    if (position.a <= 0) { return; }

    /// tangent space -> view space
    vec3 rotateCoefficient = getNoise();
    vec3 tangent  = normalize(rotateCoefficient - normal * dot(rotateCoefficient, normal));
    vec3 binormal = cross(normal, tangent);
    mat3 tbn      = mat3(tangent, binormal, normal);

    float occulusion = 0.0f;
    for (int i = 0; i < NUM_SAMPLES; ++i) {

        /// Get sample point in view space
        vec3 samplePosition = tbn * u_SamplePoints[i];
        samplePosition = position.xyz + samplePosition * RADIUS;

        /// Get sample point's uv and obtain actual screen-space point
        vec4 offsetUV      = u_Proj * vec4(samplePosition, 1.0);
             offsetUV.xyz /= offsetUV.w;
             offsetUV.xy   = offsetUV.xy * 0.5 + 0.5;
        vec4 acturalPosition = getViewPos(offsetUV.xy);

        /// Compare depth value
        if(acturalPosition.z < samplePosition.z + MIN_OCCULUSION_DIST){
            continue;
        }

        float intensity = smoothstep( 0,
                                      1,
                                      RADIUS / abs(position.z - acturalPosition.z));
        occulusion  += intensity;
    }
    occulusion /= NUM_SAMPLES;
    occulusion = 1.0f - occulusion;
    color = vec4(vec3(occulusion), 1);
    return;
    //color = texture(u_AlbedoMap, fs_UV);
    //return;
    color = vec4(color.xyz * (occulusion), 1);
}
