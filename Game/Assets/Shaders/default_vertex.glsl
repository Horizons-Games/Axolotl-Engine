#version 440
layout(location=0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertex_uv0;

layout(location = 0) uniform mat4 proj;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 model;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec2 uv0;
//out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
//out vec3 ViewPos;

void main()
{
    FragPos = vec3(model * vec4(pos, 1.0));
    Normal = mat3(transpose(inverse(model))) * normal;  

    gl_Position = proj*view*model*vec4(vertexPosition, 1.0); 

	uv0 = vertex_uv0;
    //TexCoord = texCoord;
    //ViewPos = viewPos;
}