#version 460 

layout(location = 0) uniform mat4 proj;
layout(location = 1) uniform mat4 view;

layout(location = 0) in vec3 vertexPos;
layout(location = 2) in vec4 vertexColor;

layout(location = 3) in vec2 texCoord;

out vec2 TexCoord;
out vec4 fragColor;

void main()
{
	gl_Position = proj*view*vec4(vertexPos, 1.0);

	fragColor = vertexColor;
	TexCoord = texCoord;
}