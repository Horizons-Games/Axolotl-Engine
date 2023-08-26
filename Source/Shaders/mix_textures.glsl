#version 460

layout(binding = 0) uniform sampler2D inTexture1;
layout(binding = 1) uniform sampler2D inTexture2;
uniform layout(binding=0, rg32f) writeonly image2D outImage;
uniform ivec2 inSize;

layout(local_size_x = 16, local_size_y = 8, local_size_z = 1) in;

void main()
{
	if (gl_GlobalInvocationID.x < inSize.x && gl_GlobalInvocationID.y < inSize.y) 
	{
		ivec2 inCoord = ivec2(gl_GlobalInvocationID.xy);
		vec4 color1 = texelFetch(inTexture1, inCoord, 0);
		vec4 color2 = texelFetch(inTexture2, inCoord, 0);
		vec4 color = normalize(color1 + color2);
		imageStore(outImage, inCoord, color);
	}
}