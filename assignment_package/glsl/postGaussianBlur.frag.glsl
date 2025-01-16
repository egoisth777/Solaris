#version 330

in vec2 fs_UV;

out vec3 color;

uniform sampler2D u_RenderedTexture;
uniform int u_Time;
uniform ivec2 u_Dimensions;

/*
A Gaussian blur effectively performs a weighted average of NxN pixels and stores the result
in the pixel at the center of that NxN box (this means N must always be odd).
The larger the blur radius, the smoother the blur will be.
Additionally, altering the weighting of the blur will increase or decrease its intensity.
If you take a look at slides 23 - 24 in the procedural color slides, you'll have a better idea of how a Gaussian blur works.

In order to achieve the image below, create a const array of 121 floats just above main(),
hard-coding into it the kernel values from the Gaussian kernel generator here.
Index into this array the same way you did your Z-buffer in hw03 to treat it like an 11x11 array.
We used a gamma value of 9 in the generator linked above.
*/

/// Some consts
const int KERNAL_EDGE_LENGTH = 11;
const int GAMMA = 9;
const float PI = 3.14159f;


float generateKernalVal(int x, int y){
    return  exp(-float(x * x + y * y) / (2 * GAMMA * GAMMA)) / (2 * PI * GAMMA * GAMMA);
}

void generateKernal(out float kernal[KERNAL_EDGE_LENGTH * KERNAL_EDGE_LENGTH]){
    /// Generate Gaussian kernal
    const int centerInd = KERNAL_EDGE_LENGTH / 2;
    float sum = 0;
    for(int row = 0; row < KERNAL_EDGE_LENGTH; ++row){
        for(int col = 0; col < KERNAL_EDGE_LENGTH; ++col){
            float val = generateKernalVal(row, col);
            kernal[row * KERNAL_EDGE_LENGTH + col] = generateKernalVal(row - centerInd, centerInd - col);
            sum += kernal[row * KERNAL_EDGE_LENGTH + col];
        }
    }

    /// Normalize the color
    for(int i = 0; i < KERNAL_EDGE_LENGTH * KERNAL_EDGE_LENGTH; ++i){
        kernal[i] /= sum;
    }
}

bool isUvInRange(vec2 uv){
    return uv.x >= 0 && uv.x <= 1 && uv.y >= 0 && uv.y <= 1;
}

void main()
{
    color = vec4(0);
    vec2 offset = 1.0f / vec2(u_Dimensions);

    /// Generate Gaussian kernal
    float kernal[KERNAL_EDGE_LENGTH * KERNAL_EDGE_LENGTH];
    generateKernal(kernal);


    /// Assign color based on the kernal
    const int centerInd = KERNAL_EDGE_LENGTH / 2;
    for(int row = 0; row < KERNAL_EDGE_LENGTH; ++row){
        for(int col = 0; col < KERNAL_EDGE_LENGTH; ++col){
            vec2 uv = fs_UV + offset * vec2(row - centerInd, centerInd - col);
            color += texture2D(u_RenderedTexture, uv) * kernal[row * KERNAL_EDGE_LENGTH + col];
        }
    }
}
