#version 440 core
layout (location = 0) in vec4 vertex_position; // <vec2 pos, vec2 tex>

layout(location = 0) uniform mat4 proj;
layout(location = 1) uniform mat4 model;
layout(location = 2) uniform mat4 view;
out vec2 TexCoords;

void main()
{
    gl_Position = view * proj * model * vec4(vertex_position.xy, 0.0, 1.0);
    TexCoords = vertex_position.zw;
}  