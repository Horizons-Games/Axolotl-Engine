#version 460

layout(binding = 0) uniform sampler2DArray inTexture;
uniform int cascadeCount;

uniform layout(binding=0, rg32f) writeonly image2DArray outImage;
uniform ivec2 inSize;

layout(local_size_x = 16, local_size_y = 8, local_size_z = 1) in;

void main()
{
	if (gl_GlobalInvocationID.x < inSize.x && gl_GlobalInvocationID.y < inSize.y) 
	{
		ivec2 inCoord = ivec2(gl_GlobalInvocationID.xy);

		for (unsigned int i = 0; i < cascadeCount; ++i)
		{
			float depth = texelFetch(inTexture, ivec3(inCoord, i), 0).r;
			float sqdDepth = depth*depth;
			imageStore(outImage, ivec3(inCoord, i), vec4(depth, sqdDepth, 0.0, 1.0));
		}
	}
}