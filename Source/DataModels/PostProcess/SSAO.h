#pragma once

#include "GL/glew.h"

#include <random>

#define KERNEL_SIZE 64
#define RANDOM_TANGENTS_ROWS 4
#define RANDOM_TANGENTS_COLS 4

class Program;

class SSAO
{
public:
	SSAO();
	~SSAO();
	
	void CleanUp();
	void InitBuffers();
	void UpdateBuffers(int width, int height);

	void CalculateSSAO(Program* program, int width, int height);
	void BlurSSAO(Program* program, int width, int height);

	void ToggleSSAO();
	bool IsEnabled();

	GLuint GetSSAOTexture() const;

	void SetTextures(GLuint positionTexture, GLuint normalTexture);

private:
	void CreateKernel();
	void CreateRandomTangents();

private:
	struct Kernel
	{
		float4 kernel[KERNEL_SIZE];
		float4 randomTangents[RANDOM_TANGENTS_ROWS][RANDOM_TANGENTS_COLS];
	};

	GLuint uboKernel = 0;
	GLuint ssaoFrameBuffer = 0;
	GLuint gSsao = 0;
	GLuint ssaoBlurFrameBuffer[2] = { 0, 0 };
	GLuint gSsaoBlured[2] = { 0, 0 };
	GLuint positionTexture= 0;
	GLuint normalTexture = 0;

	Kernel kernel;

	bool enabled;
};

inline void SSAO::ToggleSSAO()
{
	enabled = !enabled;
}

inline bool SSAO::IsEnabled()
{
	return enabled;
}

inline GLuint SSAO::GetSSAOTexture() const
{
	return this->gSsaoBlured[1];
}

inline void SSAO::SetTextures(GLuint positionTexture, GLuint normalTexture)
{
	this->positionTexture = positionTexture;
	this->normalTexture = normalTexture;
}

