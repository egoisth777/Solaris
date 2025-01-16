#version 330
in vec2 fs_UV;
uniform int u_Time;
uniform sampler2D u_AlbedoMap;

out vec4 color;




// generate radomly distributed (psedu) points for worley noise
// Source: ppt from NoiseFunctions
vec2 random2(vec2 p){
    return fract(sin(vec2(dot(p, vec2(127.1, 311.7)),dot(p, vec2(269.5,183.3)))) * 43758.5453);
}


// source: ppt from NoiseFunctions
float worleyNoise(vec2 uv) {
    uv *= 10.0; // Now the space is 10x10 instead of 1x1. Change this to any number you want.
    vec2 uvInt = floor(uv);
    vec2 uvFract = fract(uv);
    float minDist = 1.0; // Minimum distance initialized to max.
    for(int y = -1; y <= 1; ++y) {
        for(int x = -1; x <= 1; ++x) {
            vec2 neighbor = vec2(float(x), float(y)); // Direction in which neighbor cell lies
            vec2 point = random2(uvInt + neighbor); // Get the Voronoi centerpoint for the neighboring cell
            vec2 diff = neighbor + point - uvFract; // Distance between fragment coord and neighbor’s Voronoi point
            float dist = length(diff);
            minDist = min(minDist, dist);
        }
    }
    return minDist;
}

float surflet(vec2 P, vec2 gridPoint) {
    // Compute falloff function by converting linear distance to a polynomial
    float distX = abs(P.x - gridPoint.x);
    float distY = abs(P.y - gridPoint.y);
    float tX = 1 - 6 * pow(distX, 5.f) + 15 * pow(distX, 4.f) - 10 * pow(distX, 3.f);
    float tY = 1 - 6 * pow(distY, 5.f) + 15 * pow(distY, 4.f) - 10 * pow(distY, 3.f);
    // Get the random vector for the grid point
    vec2 gradient = 2.f * random2(gridPoint) - vec2(1.f);
    // Get the vector from the grid point to P
    vec2 diff = P - gridPoint;
    // Get the value of our height field by dotting grid->P with our gradient
    float height = dot(diff, gradient);
    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * tX * tY;
}

float perlinNoise(vec2 uv) {
        float surfletSum = 0.f;
        // Iterate over the four integer corners surrounding uv
        for(int dx = 0; dx <= 1; ++dx) {
                for(int dy = 0; dy <= 1; ++dy) {
                        surfletSum += surflet(uv, floor(uv) + vec2(dx, dy));
                }
        }
        return surfletSum;
}

// create nolstalgic effect
vec3 fadeAndRedTint(vec3 col) {
    vec3 redTint = vec3(1.0, 0.2, 0.2); // Reddish tint
    float fadeFactor = 0.4;
    float redFactor = 0.4;

    // Blend the color with the tint
    col = mix(col, redTint, redFactor + perlinNoise(fs_UV * 24.0 + vec2(u_Time * 0.005)) * 0.1);

    // Reduce the saturation to fade the color
    return col;
}


void main()
{
    // Deform the UV coordinates for a flowing effect
    vec2 deformedUV = fs_UV + vec2(perlinNoise(fs_UV  * 24.0 + vec2(u_Time * 0.005)) * 0.05);

    vec3 baseColor = texture(u_AlbedoMap, deformedUV).xyz;
    baseColor = fadeAndRedTint(baseColor);

    float perlinValue = perlinNoise(fs_UV + vec2(sin(u_Time * 0.005))) * 2.0 - 1.0; // Enhance contrast

    // Modify the base color with the noise values for a more vibrant effect
    // vec3 lavaColor = mix(baseColor, vec3(1.0, 0.0, 0.0), perlinValue * 0.5 + 0.5); // Mix with a vibrant red

    // Set the final color
    color = vec4(clamp(baseColor, 0.0, 1.0), 1.0);
}
