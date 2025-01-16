#version 330

uniform sampler2D u_AlbedoMap;

in vec2 fs_UV;

out vec4 out_Col;


void main()
{
    vec4 baseCol = texture(u_AlbedoMap, fs_UV);

    out_Col = baseCol;
}
