#version 440
layout(location = 0) in vec4 vertex_position;

layout(location = 0) uniform mat4 proj;
layout(location = 1) uniform mat4 model;
layout(location = 2) uniform mat4 view;

out vec2 uv0;
void main()
{
 gl_Position = view * proj * model * vec4(vertex_position.xy, 0.0, 1.0);
 uv0 = vertex_position.zw;
}
