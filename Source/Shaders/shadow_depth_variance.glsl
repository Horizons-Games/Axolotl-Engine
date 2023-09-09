#version 460

layout(binding = 0) uniform sampler2DArray inTexture;

uniform layout(binding=0, rg32f) writeonly image2DArray outImage;
uniform ivec2 inSize;

layout(local_size_x = 16, local_size_y = 8, local_size_z = 1) in;

void main()
{
	if (gl_GlobalInvocationID.x < inSize.x && gl_GlobalInvocationID.y < inSize.y) 
	{
		ivec2 inCoord = ivec2(gl_GlobalInvocationID.xy);

		float depth0 = texelFetch(inTexture, ivec3(inCoord, 0), 0).r;
		float depth1 = texelFetch(inTexture, ivec3(inCoord, 1), 0).r;
		float sqdDepth0 = depth0*depth0; 
		float sqdDepth1 = depth1*depth1;

		imageStore(outImage, ivec3(inCoord, 0), vec4(depth0, sqdDepth0, 0.0, 1.0));
		imageStore(outImage, ivec3(inCoord, 1), vec4(depth1, sqdDepth1, 0.0, 1.0));
	}
}