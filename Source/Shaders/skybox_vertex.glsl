#version 440
layout(location = 0) in vec3 aPos;

layout(location = 0) uniform mat4 proj;
layout(location = 1) uniform mat4 view;

out vec3 TexCoords;

void main()
{
    TexCoords = aPos;
    gl_Position = (proj * vec4(mat3(view) * aPos, 1.0));
}