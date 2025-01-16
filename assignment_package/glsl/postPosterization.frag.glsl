#version 330
// Posterization Effect

in vec2 fs_UV;

out vec4 color;

uniform sampler2D u_AlbedoMap;

const float LEVEL = 20.f;       // Determine the rounding color level

float calculateGrey(vec3 color){
    //float grey = dot(color, vec3(0.21, 0.72, 0.07));
    float grey = max(color.r, max(color.g, color.b));

    return grey;
}


float calculateLevel(float val, float levelCount){
    float lower = floor(val * levelCount) / levelCount;
    float lowerDist = val - lower;

    float upper = ceil(val * levelCount) / levelCount;
    float upperDist = upper - val;

    return upperDist > lowerDist ? upper : lower;
}

float calculateToonAdjustment(vec3 color){

    float greyScale = calculateGrey(color);
    return calculateLevel(greyScale, LEVEL);
}

void main() {
    color = texture(u_AlbedoMap, fs_UV);
    float adjustment = calculateToonAdjustment(color.rgb);
    color.rgb *= adjustment;
}
