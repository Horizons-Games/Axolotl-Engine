#version 460
  
in vec2 TexCoord;

layout(binding = 0) uniform sampler2D image;
  
out vec4 outColor;

void main()
{
	vec2 texSize = vec2(textureSize(image, 0));
	
	vec2 halfT = 0.5/texSize;
	
	vec4 sum = texture(image, TexCoord + vec2(-halfT.x * 2.0, 0.0));
	sum += texture(image, TexCoord + vec2(-halfT.x, halfT.y)) * 2.0;
	sum += texture(image, TexCoord + vec2(0.0, halfT.y * 2.0));
	sum += texture(image, TexCoord + vec2(halfT.x, halfT.y)) * 2.0;
	sum += texture(image, TexCoord + vec2(halfT.x * 2.0, 0.0));
	sum += texture(image, TexCoord + vec2(halfT.x, -halfT.y)) * 2.0;
	sum += texture(image, TexCoord + vec2(0.0, -halfT.y * 2.0));
	sum += texture(image, TexCoord + vec2(-halfT.x, -halfT.y)) * 2.0;
	
	outColor = sum / 12.0;
}