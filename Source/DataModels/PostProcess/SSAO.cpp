#include "StdAfx.h"
#include "SSAO.h"

#include "AxoLog.h"

#include "Program/Program.h"

SSAO::SSAO()
{
	CreateKernel();
	CreateRandomTangents();
}

SSAO::~SSAO()
{
	CleanUp();
}

void SSAO::CleanUp()
{
	glDeleteFramebuffers(1, &ssaoFrameBuffer);
	glDeleteTextures(1, &gSsao);
}

void SSAO::InitBuffers()
{
	glGenFramebuffers(1, &ssaoFrameBuffer);
	glGenTextures(1, &gSsao);

	glGenBuffers(1, &uboKernel);
	glBindBuffer(GL_UNIFORM_BUFFER, uboKernel);

	int size = sizeof(kernel);
	glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 1, uboKernel, 0, sizeof(kernel));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void SSAO::UpdateBuffers(int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFrameBuffer);

	glBindTexture(GL_TEXTURE_2D, gSsao);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gSsao, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("ERROR::FRAMEBUFFER:: SSAO framebuffer not completed!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void SSAO::Render(Program* program, int width, int height)
{
	program->Activate();

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, std::strlen("SSAO"), "SSAO");

	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFrameBuffer);
	
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, positionTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalTexture);

	program->BindUniformFloat2("screenSize", float2(width, height));

	glBindBuffer(GL_UNIFORM_BUFFER, uboKernel);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float3) * KERNEL_SIZE, &kernel.kernel);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float3) * KERNEL_SIZE, sizeof(float3) * RANDOM_ROT_SIZE,
				    &kernel.randomTangents);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glPopDebugGroup();
	
	program->Deactivate();
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

		kernel.kernel[i] = dir;
	}
}

void SSAO::CreateRandomTangents()
{
	std::uniform_real_distribution<float> randoms(0.0f, 1.0f);
	std::default_random_engine generator;

	for (unsigned i = 0; i < RANDOM_ROT_SIZE; ++i)
	{
		float3 dir;

		dir.x = randoms(generator) * 2.0f - 1.0f;
		dir.y = randoms(generator) * 2.0f - 1.0f;
		dir.z = 0.0f;

		dir.Normalize();

		kernel.randomTangents[i] = dir;
	}

}
