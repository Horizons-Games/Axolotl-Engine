#version 460

layout(binding = 0) uniform sampler2DArray sourceTexture;
uniform vec2 invSize;
uniform vec2 blurDirection;

in vec2 TexCoord;
out vec4 color;

const int SAMPLE_COUNT = 3;
const float OFFSETS[3] = float[3](
    -1.4990620619239194,
    0.4996866407382734,
    2
);

const float WEIGHTS[3] = float[3](
    0.3997495607369067,
    0.4007505992285515,
    0.19949984003454174
);

void main()
{
	vec4 result = vec4(0.0);

	for (int i = 0; i < SAMPLE_COUNT; ++i)
	{
		result += texture(sourceTexture, vec3(TexCoord + blurDirection * OFFSETS[i] * invSize, gl_Layer)) * WEIGHTS[i];
	}

	color = vec4(result.rgb, 1.0);
}
