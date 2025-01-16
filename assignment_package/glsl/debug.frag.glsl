#version 330

uniform sampler2D u_AlbedoMap;

in vec2 fs_UV;

out vec4 out_Col;

float LinearizeDepth(float depth)
{
    float near = 0.1f;
    float far = 250.f;
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    //float depth = texture(u_AlbedoMap, vec3(fs_UV, 1)).r;
    //depth = LinearizeDepth(depth);

    float depth = texture(u_AlbedoMap, fs_UV).r;
    out_Col = vec4(vec3(depth), 1.0);
}
