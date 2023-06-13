#version 460


in vec2 TexCoord;
out vec4 outcolor;

layout(location = 0) uniform sampler2D tex;

void main()
{
    outcolor = texture2D(tex, TexCoord);
}