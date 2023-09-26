#include "StdAfx.h"
#include "SSAO.h"

#include "Auxiliar/Utils/UtilBlur.h"

#include "Program/Program.h"

#define BIAS 0.05f
#define RADIUS 1.5f

SSAO::SSAO() : enabled(true)
{
	uboKernel = 0;
	ssaoFrameBuffer = 0;
	gSsao = 0;
	positionTexture = 0;
	normalTexture = 0;

	CreateKernel();
	CreateRandomTangents();

	utilBlur = new UtilBlur();
}

SSAO::~SSAO()
{
	delete utilBlur;
	CleanUp();
}

void SSAO::CleanUp()
{
	glDeleteFramebuffers(1, &ssaoFrameBuffer);
	glDeleteTextures(1, &gSsao);

	glDeleteTextures(1, &gSsaoBlured);
}

void SSAO::InitBuffers()
{
	glGenFramebuffers(1, &ssaoFrameBuffer);
	glGenTextures(1, &gSsao);

	glGenTextures(1, &gSsaoBlured);

	int size = sizeof(kernel);

	glGenBuffers(1, &uboKernel);
	glBindBuffer(GL_UNIFORM_BUFFER, uboKernel);
	glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 5, uboKernel, 0, size);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void SSAO::UpdateBuffers(int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFrameBuffer);

	glBindTexture(GL_TEXTURE_2D, gSsao);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gSsao, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("ERROR::FRAMEBUFFER:: SSAO framebuffer not completed!");
	}

	glBindTexture(GL_TEXTURE_2D, gSsaoBlured);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		SDL_assert(SDL_FALSE && "Problem texture Blur SSAO");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void SSAO::CalculateSSAO(Program* program, int width, int height)
{
	program->Activate();

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<GLsizei>(std::strlen("SSAO")), "SSAO");

	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFrameBuffer);
	
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, positionTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalTexture);

	program->BindUniformFloat2("screenSize", float2(static_cast<float>(width), static_cast<float>(height)));
	program->BindUniformFloat("bias", BIAS);
	program->BindUniformFloat("radius", RADIUS);

	glBindBuffer(GL_UNIFORM_BUFFER, uboKernel);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(kernel), &kernel, GL_STATIC_DRAW);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glPopDebugGroup();
	
	program->Deactivate();
}

void SSAO::BlurSSAO(int width, int height)
{
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<GLsizei>(std::strlen("SSAO - Gaussian Blur")),
		"SSAO - Gaussian Blur");
	utilBlur->BlurTexture(gSsao, gSsaoBlured, GL_R16F, GL_RED, GL_FLOAT, 0, width, height, 0, width, height);
	glPopDebugGroup();
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

		kernel.kernel[i] = float4(dir, 0.0f);
	}
}

void SSAO::CreateRandomTangents()
{
	std::uniform_real_distribution<float> randoms(0.0f, 1.0f);
	std::default_random_engine generator;

	for (unsigned i = 0; i < RANDOM_TANGENTS_ROWS; ++i)
	{
		for (unsigned j = 0; j < RANDOM_TANGENTS_ROWS; ++j)
		{
			float3 dir;

			dir.x = randoms(generator) * 2.0f - 1.0f;
			dir.y = randoms(generator) * 2.0f - 1.0f;
			dir.z = 0.0f;

			dir.Normalize();

			kernel.randomTangents[i][j] = float4(dir, 0.0f);
		}
	}
}
