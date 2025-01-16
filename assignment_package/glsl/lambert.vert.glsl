#version 150
// ^ Change this to version 130 if you have compatibility issues

uniform mat4 u_Model;       // The matrix that defines the transformation of the
                            // object we're rendering. In this assignment,
                            // this will be the result of traversing your scene graph.

uniform mat4 u_ModelInvTr;  // The inverse transpose of the model matrix.
                            // This allows us to transform the object's normals properly
                            // if the object has been non-uniformly scaled.

uniform mat4 u_ViewProj;    // The matrix that defines the camera's transformation.
                            // We've written a static matrix for you to use for HW2,
                            // but in HW3 you'll have to generate one yourself
uniform mat4 u_ViewProjLastFrame;
uniform vec4 u_Color;       // When drawing the cube instance, we'll set our uniform color to represent different block types.

uniform int u_Time;

in vec4 vs_Pos;             // The array of vertex positions passed to the shader

in vec4 vs_Nor;             // The array of vertex normals passed to the shader

in vec2 vs_UV;

in vec4 vs_Col;             // The array of vertex colors passed to the shader.

in int vs_Type;

out vec4 fs_Pos;
out vec4 fs_Nor;            // The array of normals that has been transformed by u_ModelInvTr. This is implicitly passed to the fragment shader.
out vec4 fs_LightVec;       // The direction in which our virtual light lies, relative to each vertex. This is implicitly passed to the fragment shader.
out vec2 fs_UV;
flat out int fs_Type;
out mat3 TBN;

const vec4 lightDir = normalize(vec4(0.2, 1, 0.8, 0));  // The direction of our virtual light, which is used to compute the shading of
                                        // the geometry in the fragment shader.
void setTBN()
{
    vec3 N = vs_Nor.rgb;
    vec3 T = normalize(cross(N, vec3(0, 1, 1)));
    vec3 B = cross(N, T);
    TBN = mat3(T,B,N);
}


void main()
{
    fs_Pos = vs_Pos;
    fs_UV = vs_UV;
    fs_Type = vs_Type;
    // case LAVA and WATER
    // uv offset
    if (vs_Type == 8 || vs_Type == 4)
    {
        float offset = float(u_Time % 1000) / 1000. * (2. / 16.);
        fs_UV.x += offset;

        // when dealing up face
        if (vs_Nor.y > 0.)
        {
            fs_Pos.y -= 0.15;
        }
    }


    mat3 invTranspose = mat3(u_ModelInvTr);
    fs_Nor = vec4(invTranspose * vec3(vs_Nor), 0);          // Pass the vertex normals to the fragment shader for interpolation.
                                                            // Transform the geometry's normals by the inverse transpose of the
                                                            // model matrix. This is necessary to ensure the normals remain
                                                            // perpendicular to the surface after the surface is transformed by
                                                            // the model matrix.
    setTBN();

    vec4 modelposition = u_Model * fs_Pos;   // Temporarily store the transformed vertex positions for use below

    fs_LightVec = (lightDir);  // Compute the direction in which the light source lies

    gl_Position = u_ViewProj * modelposition;// gl_Position is a built-in variable of OpenGL which is
                                             // used to render the final positions of the geometry's vertices
}
