#pragma once

#include "GL/glew.h"

#include <random>

#define KERNEL_SIZE 10
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

	void Render(Program* program, int width, int height);

	void SetTextures(GLuint depthTexture, GLuint positionTexture, GLuint normalTexture);

private:
	void CreateKernel();
	void CreateRandomTangents();

private:
	struct Kernel
	{
		float3 kernel[KERNEL_SIZE];
		float3 randomTangents[RANDOM_TANGENTS_ROWS][RANDOM_TANGENTS_COLS];
	};

	GLuint uboKernel = 0;
	GLuint ssaoFrameBuffer = 0;
	GLuint gSsao = 0;
	GLuint depthTexture = 0;
	GLuint positionTexture= 0;
	GLuint normalTexture = 0;

	Kernel kernel;
};

inline void SSAO::SetTextures(GLuint depthTexture, GLuint positionTexture, GLuint normalTexture)
{
	this->depthTexture = depthTexture;
	this->positionTexture = positionTexture;
	this->normalTexture = normalTexture;
}

