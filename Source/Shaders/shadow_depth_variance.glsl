#version 460

layout(binding = 0) uniform sampler2D inTexture;

uniform layout(binding=0, rg32f) writeonly image2D outImage;
uniform ivec2 inSize;

layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;

void main()
{
	if (gl_GlobalInvocationID.x < inSize.x && gl_GlobalInvocationID.y < inSize.y) 
	{
		ivec2 inCoord = ivec2(gl_GlobalInvocationID.xy);

		float depth = texelFetch(inTexture, inCoord, 0).r;
		float sqdDepth = depth*depth; 

		imageStore(outImage, inCoord, vec4(depth, sqdDepth, 0.0, 1.0));
	}
}