#version 460

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

layout(std140, row_major, binding = 0) uniform Camera 
{
    mat4 proj;      // 16 // 00 (column 0)
                    // 16 // 16 (column 1)
                    // 16 // 32 (column 2)
                    // 16 // 48 (column 3)
    mat4 view;      // 16 // 64 (column 0)
                    // 16 // 80 (column 1)
                    // 16 // 96 (column 2)
                    // 16 // 112 (column 3)
};

uniform mat4 model;


void main()
{
    gl_Position = proj*view*model*vec4(position, 1.0);
}