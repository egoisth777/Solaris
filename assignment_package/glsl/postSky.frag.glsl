#version 330
// Bloom Effect

in vec2 fs_UV;

out vec4 color;

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_SkyMap;

uniform bool u_EnableShader;

void main() {
    vec4 albedo = texture(u_AlbedoMap, fs_UV);
    if(!u_EnableShader){
        color = albedo;
        return;
    }

    vec4 skyColor = texture(u_SkyMap, fs_UV);
    color = albedo.a <= 0 ? skyColor : albedo;
}
