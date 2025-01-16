# version 330
uniform ivec2 u_Dimensions;
in vec2 fs_UV;
uniform int u_Time;
uniform sampler2D u_AlbedoMap;
out vec4 color;

// noise function
// Source :lecture pptx from NoiseFunction PPTX
// generate random points across the screen
vec2 random2(vec2 p){
       return fract(sin(vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)))) * 45758.5453);
}


float interpNoise2D(float x, float y) {
   int intX = int(floor(x));
   float fractX = fract(x);
   int intY = int(floor(y));
   float fractY = fract(y);

   float v1 = random2(vec2(intX, intY)).x;
   float v2 = random2(vec2(intX + 1, intY)).x;
   float v3 = random2(vec2(intX, intY + 1)).x;
   float v4 = random2(vec2(intX + 1, intY + 1)).x;

   float i1 = mix(v1, v2, fractX);
   float i2 = mix(v3, v4, fractX);
   return mix(i1, i2, fractY);
}


// 2D FBM function that
float fbm(float x, float y) {
   float total = 0;
   float persistence = 0.5f;
   int octaves = 8;
   float freq = 2.f;
   float amp = 0.5f;
   for(int i = 1; i <= octaves; i++) {
       total += interpNoise2D(x * freq,
                              y * freq) * amp;

       freq *= 2.f;
       amp *= persistence;
   }
   return total;
}



// Source: lecture pptx from NoiseFunction PPTX
// return the mindist of the current pixel to the randomly distributed point
float WorleyNoise(vec2 uv) {
  uv.x *= 30.0 * abs(fract(sin(0.001 * u_Time))); // Now the space is 10x10 instead of 1x1. Change this to any number you want.
  uv.y *= 30.0 * abs(fract(sin(0.001 * u_Time)));
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

// adjust the luminance of the input pixel
vec3 adjustLuminance(vec3 c, float adjustment){
    float l = dot (c, vec3(0.299, 0.587, 0.114));
    return c * adjustment;
}

// Rotate function
vec2 rotate(vec2 v, float a) {
   float s = sin(a);
   float c = cos(a);
   mat2 m = mat2(c, -s, s, c);
   return m * v;
}

vec2 getOffset(vec2 uv) {
   return vec2(fbm(uv.x, uv.y) * 2.0 - 1.0, fbm(rotate(uv, 30.0).x, rotate(uv, 30.0).y) * 2.0 - 1.0);
}



float intervalRemapper(float target, float curr_lower_bound, float curr_upper_bound, float target_lower_bound, float target_upper_bound){
    return (target - curr_lower_bound) / (curr_upper_bound - curr_lower_bound) * (target_upper_bound - target_lower_bound) + target_lower_bound;
}

void main()
{
   vec2 deformed_uv = fs_UV + vec2(perlinNoise(fs_UV + vec2(sin(u_Time * 0.001))) * 0.1);
   vec2 uv = fs_UV * getOffset(fs_UV); // adding offset to the uv values
   vec3 baseCol = texture(u_AlbedoMap, deformed_uv).xyz;

   float worleyValue = intervalRemapper(WorleyNoise(uv), 0.0, 1.0, 0.8, 1.0); //clamp(WorleyNoise(uv), 0.6, 1.0);

   baseCol = adjustLuminance(baseCol, worleyValue);
   float alpha = 0.9;

   color = vec4(baseCol, alpha);
}
