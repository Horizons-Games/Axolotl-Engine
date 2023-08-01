#version 460

#extension GL_ARB_shading_language_include : require

#include "/Common/Functions/pbr_functions.glsl"

out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = SRGBA(texture(skybox, TexCoords));
}