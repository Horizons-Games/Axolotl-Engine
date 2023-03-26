#version 440
layout(location = 0) in vec4 vertex_position;
out vec2 uv0;
void main()
{
 gl_Position = vec4(vertex_position.xy, 0.0, 1.0);
 uv0 = vertex_position.zw;
}
