#version 460

#define KERNEL_SIZE 10
#define TANGENT_ROWS 5
#define TANGENT_COLS 5

in vec2 uv;
out vec4 color;

uniform sampler2D positions;
uniform sampler2D normals;

uniform vec2 screenSize;
uniform vec3 randomTangents[TANGENT_ROWS][TANGENT_COLS];
uniform vec3 kernelSamples[KERNEL_SIZE];

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

mat3 createTangentSpace(const vec3 normal, const vec3 randTangent)
{
	vec3 tangent = normalize(randTangent-normal*dot(normal, randTangent)); // Gram-Schmidt
	vec3 bitangent = normalize(cross(tangent, normal));
	return mat3(bitangent, tangent, normal);
}

vec3 getRandomTangent()
{
	vec2 screenPos = uv*screenSize;
	ivec2 index = ivec2(int(mod(screenPos.y, TANGENT_ROWS)), int(mod(screenPos.x, TANGENT_COLS)));
	return randomTangents[index.x][index.y];
}

float getSceneDepthAtSamplePos(in vec3 samplePos)
{
	vec4 clippingSpace = projection*vec4(samplePos, 1.0);
	vec2 sampleUV = (clippingSpace.xy/clippingSpace.w)*0.5+0.5;

	return (camera_view*texture(positions, sampleUV)).z;
}

void main()
{
	vec3 position = camera_view*texture(positions, uv).xyz;
	vec3 normal = mat3(camera_view)*normalize(texture(normals, uv).xyz);
	mat3 tangentSpace = createTangentSpace(normal, getRandomTangent());
	int occlusion = 0;


	for(int i=0; i< KERNEL_SIZE; ++i)
	{
		vec3 samplePos = position+tangentSpace*kernelSamples[i];
		if(getSceneDepthAtSamplePos(samplePos) > samplePos.z) ++occlusion;
	}

	color = vec4(vec3(1.0-float(occlusion)/float(KERNEL_SIZE)), 1.0f);
}