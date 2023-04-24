#version 460

layout (location = 0) uniform mat4 view;
layout (location = 1) uniform mat4 proj;

layout (location = 0) in vec3 pos;

out vec3 texcoords;

void main()
{
	texcoords = vec3(-pos.x, pos.yz);
	mat3 rotView = mat3(view); // remove translation from the view matrix
	vec4 p = proj * vec4(rotView * pos, 1.0);
	gl_Position = p.xyww;
}