#version 460

#define PARTITIONS 2

uniform vec2 nearFar;
uniform int splits;
uniform float lambda;

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) writeonly buffer MyBuffer
{
	float splitPositions[PARTITIONS];
};

void main()
{
	float near = nearFar[0];
	float far = nearFar[1];

	float logarithmicSplit = near * pow(far/near, (gl_WorkGroupID.x + 1.0)/(splits + 1.0));
	float uniformSplit = near + (far - near) * (float(gl_WorkGroupID.x + 1) / float(splits + 1));
	float splitPosition = lambda * logarithmicSplit + (1.0 - lambda) * uniformSplit;

	splitPositions[gl_WorkGroupID.x] = splitPosition;
}