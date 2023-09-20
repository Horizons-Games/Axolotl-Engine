#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D videoTexture; // Texture where the video frame is loaded

void main()
{
    FragColor = texture(videoTexture, TexCoord);
}