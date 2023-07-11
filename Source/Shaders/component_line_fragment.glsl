#version 460

layout(binding=0) uniform sampler2D textureColor;

in vec2 TexCoord;
in vec3 incolor;

uniform int hasTexture;

out vec4 outColor;
  
void main()
{
    if(hasTexture == 1)
    {
    outColor = texture(textureColor, TexCoord);
    outColor.rgb *= incolor;
    }
    else
    {
        outColor = vec4(incolor,1.0f);
    }
}