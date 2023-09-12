#version 460

uniform vec2 nearFar;
uniform float splits;
uniform float lambda;

layout(local_size_x = 8, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding =3) writeonly buffer MyBuffer
{
	float splitPositions[gl_WorkGroupSize.x];
};

void main()
{
	if(gl_GlobalInvocationID.x < splits)
	{
		float near = nearFar[0];
		float far = nearFar[1];

		float logarithmicSplit = near * pow(far/near, (gl_GlobalInvocationID.x + 1.0)/(splits + 1.0));
		float uniformSplit = near + (far - near) * (float(gl_GlobalInvocationID.x + 1) / float(splits + 1));
		float splitPosition = lambda * logarithmicSplit + (1.0 - lambda) * uniformSplit;

		splitPositions[gl_GlobalInvocationID.x] = splitPosition;
	}
}