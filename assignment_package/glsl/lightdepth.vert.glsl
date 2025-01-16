#version 330

uniform mat4 u_Model;

uniform mat4 u_ViewProj;

in vec4 vs_Pos;

in vec2 vs_UV;

out vec2 fs_UV;

void main()
{
    fs_UV = vs_UV;
    // world
    vec4 pos = u_Model * vs_Pos;

    // light space
    vec4 glpos = u_ViewProj * pos;
    glpos.z = max(-1.f, glpos.z);
    gl_Position = glpos;
}
