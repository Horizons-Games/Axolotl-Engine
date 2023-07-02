#version 460

uniform sampler2D textureColor;

in vec2 TexCoord;
in vec3 incolor;

out vec4 outColor;
  
void main()
{
    outColor = texture(textureColor, TexCoord);
    outColor.rgb *= incolor;
}