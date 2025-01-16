#version 330
// Bloom Effect

in vec2 fs_UV;

out vec4 color;

uniform sampler2D u_AlbedoMap;

const float THRESHOLD = 0.9f;
const int   SAMPLE_SIZE = 10;
const float SAMPLE_STEP_SIZE = 3.0f;
const float INTENSITY = .3f;

float calculateGrey(vec3 color){
    //float grey = dot(color, vec3(0.21, 0.72, 0.07));
    float grey = max(color.r, max(color.g, color.b));

    return grey;
}

void main() {
    color = texture(u_AlbedoMap, fs_UV);
    if (SAMPLE_SIZE <= 0) {
        return;
    }

    vec2 dimension = textureSize(u_AlbedoMap, 0).xy;

    vec4 result = vec4(0.0);
    vec4 sampleColor  = vec4(0.0);


    for (int i = -SAMPLE_SIZE; i <= SAMPLE_SIZE; ++i) {
        for (int j = -SAMPLE_SIZE; j <= SAMPLE_SIZE; ++j) {

            sampleColor = texture(
                        u_AlbedoMap,
                        fs_UV + (vec2(i, j) * SAMPLE_STEP_SIZE) / dimension);


            if (calculateGrey(sampleColor.rgb) < THRESHOLD) {
                continue;
            }

            result += sampleColor;
        }
    }

    int sampleCount = SAMPLE_SIZE * 2 + 1;
    result /= float(sampleCount * sampleCount);
    result = mix(vec4(0.0), result, INTENSITY);
//    color = result;
//    return;
    color.rgb += result.rgb;
}
