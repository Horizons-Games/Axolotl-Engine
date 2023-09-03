#include "StdAfx.h"
#include "SSAO.h"

SSAO::SSAO(GLuint depthBuffer, GLuint positionBuffer, GLuint normalBuffer) :
	depthBuffer(depthBuffer),
	positionBuffer(positionBuffer),
	normalBuffer(normalBuffer)
{
	kernel.reserve(KERNEL_SIZE);
	randomTangents.reserve(RANDOM_TANGENTS_ROWS * RANDOM_TANGENTS_COLS);
}

SSAO::~SSAO()
{
	kernel.clear();
	randomTangents.clear();
}

void SSAO::CreateKernel()
{
	std::uniform_real_distribution<float> randoms(0.0f, 1.0f);
	std::default_random_engine generator;

	for (unsigned i = 0; i < KERNEL_SIZE; ++i)
	{
		float3 dir;
		dir.x = randoms(generator) * 2.0f - 1.0f;
		dir.y = randoms(generator) * 2.0f - 1.0f;
		dir.z = randoms(generator);

		dir.Normalize();

		dir *= randoms(generator); // random size

		// More sample points near the origin
		float scale = float(i) / float(KERNEL_SIZE);

		scale = 0.1f + (scale * scale) * (1.0f - 0.1f);
		dir *= scale;
		
		kernel[i] = dir;
	}
}

void SSAO::CreateRandomTangents()
{
	std::uniform_real_distribution<float> randoms(0.0f, 1.0f);
	std::default_random_engine generator;

	for (unsigned i = 0; i < RANDOM_TANGENTS_ROWS; ++i)
	{
		for (unsigned j = 0; j < RANDOM_TANGENTS_COLS; ++j)
		{
			float3 dir;

			dir.x = randoms(generator) * 2.0f - 1.0f;
			dir.y = randoms(generator) * 2.0f - 1.0f;
			dir.z = 0.0f;
			
			dir.Normalize();
			
			randomTangents[i][j] = dir;
		}
	}

}

