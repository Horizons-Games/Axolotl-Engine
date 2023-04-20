#version 440

layout (location = 0) in vec3 pos;

uniform mat4 projection;
uniform mat4 view;

out vec3 texCoords;

void main()
{
    texCoords = pos;  
    gl_Position =  projection * view * vec4(texCoords, 1.0);
}
