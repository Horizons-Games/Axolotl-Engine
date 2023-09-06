#version 460

#extension GL_ARB_bindless_texture : require
#extension GL_ARB_shading_language_include : require

#define KERNEL_SIZE 64
#define TANGENT_ROWS 4
#define TANGENT_COLS 4

in vec2 TexCoord;
out vec4 color;

uniform vec2 screenSize;
uniform float bias;
uniform float radius;

layout(binding = 0) uniform sampler2D gPositions;
layout(binding = 1) uniform sampler2D gNormals;
layout(binding = 2) uniform sampler2DShadow gDepth;

layout(std140, row_major, binding = 0) uniform Camera 
{
    mat4 proj;      // 16 // 00 (column 0)
                    // 16 // 16 (column 1)
                    // 16 // 32 (column 2)
                    // 16 // 48 (column 3)
    mat4 view;      // 16 // 64 (column 0)
                    // 16 // 80 (column 1)
                    // 16 // 96 (column 2)
                    // 16 // 112 (column 3)
};

layout(std140, binding = 5) uniform Kernel 
{
	vec3 kernelSamples[KERNEL_SIZE];
	vec3 randomTangents[TANGENT_ROWS][TANGENT_COLS];
};

mat3 createTangentSpace(const vec3 normal, const vec3 randTangent)
{
	vec3 tangent = normalize(randTangent-normal*dot(randTangent, normal)); // Gram-Schmidt
	vec3 bitangent = normalize(cross(normal, tangent));
	return mat3(tangent, bitangent, normal);
}

vec3 getRandomTangent()
{
	vec2 screenPos = TexCoord*screenSize;
	ivec2 index = ivec2(int(mod(screenPos.y, TANGENT_ROWS)), int(mod(screenPos.x, TANGENT_COLS)));
	return randomTangents[index.x][index.y];
}

float getSceneDepthAtSamplePos(in vec3 samplePos)
{
	vec4 clippingSpace = proj*vec4(samplePos, 1.0);
	vec2 sampleUV = (clippingSpace.xy/clippingSpace.w)*0.5+0.5;

	return (view*texture(gPositions, sampleUV)).z;
}

void main()
{
	vec3 position = (view*vec4(texture(gPositions, TexCoord).xyz, 1.0)).xyz;
	vec3 normal = mat3(view)*normalize(texture(gNormals, TexCoord).xyz);
	mat3 tangentSpace = createTangentSpace(normal, getRandomTangent());
	float occlusion = 0.0;

	for(int i=0; i< KERNEL_SIZE; ++i)
	{
		vec3 samplePos = position+tangentSpace*kernelSamples[i];
		float sampleDepth = getSceneDepthAtSamplePos(samplePos);
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(position.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
	}

	color = vec4(vec3(1.0-occlusion/float(KERNEL_SIZE)), 1.0f);
}