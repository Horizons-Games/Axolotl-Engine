#version 460

layout(binding = 0) uniform sampler2D inTexture;

layout(std430, binding = 1) writeonly buffer MinMax
{
	vec2 data;
};

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

void main()
{
	data = texelFetch(inTexture, ivec2(0, 0), 0).rg;
}