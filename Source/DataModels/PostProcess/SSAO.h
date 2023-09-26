#pragma once

#include "GL/glew.h"

#include <random>

#define KERNEL_SIZE 64
#define RANDOM_TANGENTS_ROWS 4
#define RANDOM_TANGENTS_COLS 4

class Program;
class UtilBlur;

class SSAO
{
public:
	SSAO();
	~SSAO();
	
	void CleanUp();
	void InitBuffers();
	void UpdateBuffers(int width, int height);

	void CalculateSSAO(Program* program, int width, int height);
	void BlurSSAO(int width, int height);

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

	GLuint uboKernel;
	GLuint ssaoFrameBuffer;
	GLuint gSsao;
	GLuint gSsaoBlured;
	GLuint positionTexture;
	GLuint normalTexture;

	Kernel kernel;

	UtilBlur* utilBlur;

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
	return gSsaoBlured;
}

inline void SSAO::SetTextures(GLuint positionTexture, GLuint normalTexture)
{
	this->positionTexture = positionTexture;
	this->normalTexture = normalTexture;
}

