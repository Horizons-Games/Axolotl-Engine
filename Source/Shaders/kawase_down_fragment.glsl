#version 460
  
in vec2 TexCoord;

layout(binding = 0) uniform sampler2D image;
  
out vec4 outColor;

void main()
{
    vec2 texSize = vec2(textureSize(image, 0));

    vec2 halfT = 0.5/texSize;

    vec4 sum = texture(image, TexCoord) * 4.0;
    sum += texture(image, TexCoord - halfT.xy);
    sum += texture(image, TexCoord + halfT.xy);
    sum += texture(image, TexCoord + vec2(halfT.x, -halfT.y));
    sum += texture(image, TexCoord - vec2(halfT.x, -halfT.y));
    
    outColor = sum/8.0;
}