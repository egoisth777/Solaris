#version 330

uniform sampler2D u_AlbedoMap;

in vec2 fs_UV;

void main()
{
    vec4 baseCol = texture(u_AlbedoMap, fs_UV);
    if (baseCol.a == 0.f)
    {
        discard;
    }
}
