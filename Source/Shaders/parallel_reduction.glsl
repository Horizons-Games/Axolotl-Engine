#version 460

layout(binding = 0) uniform sampler2D inTexture;
uniform layout(binding=0, rg32f) writeonly image2D outImage;
uniform ivec2 inSize;

layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
shared vec2 values[gl_WorkGroupSize.x*gl_WorkGroupSize.y];

void main()
{
	if(gl_GlobalInvocationID.x < inSize.x && gl_GlobalInvocationID.y < inSize.y) // work item inside work domain
	{
		ivec2 inCoord = ivec2(gl_GlobalInvocationID.xy);
		// Load depth value for all work items
		values[gl_LocalInvocationIndex] = texelFetch(inTexture, inCoord, 0).rg;
	}
	else
	{
		values[gl_LocalInvocationIndex] = vec2(1.0, 0.0);
	}
	
	// Synchronization
	memoryBarrierShared(); // ensure memory is visible
	barrier(); // ensure all barriers arrived to here
	
	// Only one work item finds the minimum (the one with index 0)
	if(gl_LocalInvocationIndex == 0)
	{
		float minValue = 1.0;
		float maxValue = 0.0;
		for(int i=0; i< int(gl_WorkGroupSize.x*gl_WorkGroupSize.y); ++i)
		{
			minValue = min(minValue, values[i].r);
			maxValue = values[i].g != 1.0 ? max(maxValue, values[i].g) : maxValue;
		}
		imageStore(outImage, ivec2(gl_WorkGroupID.xy), vec4(minValue, maxValue, 0.0, 0.0));
	}
}