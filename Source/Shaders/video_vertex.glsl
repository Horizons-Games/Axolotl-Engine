#version 330 core

layout(location = 0) in vec3 inPosition;    // Vertex position
layout(location = 1) in vec2 inTexCoord;    // Texture coordinate

uniform mat4 model;       // Model matrix
uniform mat4 view;        // View matrix
uniform mat4 projection;  // Projection matrix

out vec2 TexCoord;        // For passing to the fragment shader

void main()
{
    gl_Position = projection * view * model * vec4(inPosition, 1.0f);
    TexCoord = inTexCoord;
}