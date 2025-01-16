#version 330

uniform float u_DepthBound[4];

uniform mat4 u_Model;

uniform mat4 u_ViewProj;

uniform vec3 u_LightDir;

uniform mat4 u_LightViewProj[3];

in vec4 vs_Pos;

in vec4 vs_Nor;

in vec2 vs_UV;

out vec4 fs_Pos;

out vec4 fs_Nor;

out vec2 fs_UV;

out vec4 fs_LightPos[3];

const float bMAx = 0.05f;

void main()
{
    fs_Pos = vs_Pos;
    fs_Nor = vs_Nor;
    fs_UV = vs_UV;

    vec4 modelposition = u_Model * vs_Pos;

    float bias = clamp(bMAx * (1.0 - dot(vec3(vs_Nor), u_LightDir)),0.001f, bMAx);
    //bias = 0.f;

    for (int i = 0; i < 3; ++i)
    {
        fs_LightPos[i] = u_LightViewProj[i] * (modelposition + bias * vs_Nor);
        // pancaking
        //fs_LightPos[i].z = max(0.f, fs_LightPos[i].z);
    }

    gl_Position = u_ViewProj * modelposition;
}
