#version 460

layout(location = 0) uniform mat4 proj;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 model;

layout(location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 texCoord;
layout(location = 2) in vec3 vertexColor;

uniform vec2 offset;
uniform vec2 tiling;

out vec2 TexCoord;
out vec3 incolor;

void main()
{
    TexCoord = texCoord*tiling+offset;

    incolor = vertexColor;

     gl_Position = view * proj * model * vec4(vertexPosition, 1.0);
}
