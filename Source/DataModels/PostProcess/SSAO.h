#pragma once

#include "GL/glew.h"

#include <random>

#define KERNEL_SIZE 10
#define RANDOM_TANGENTS_ROWS 5
#define RANDOM_TANGENTS_COLS 5

class SSAO
{
public:
	SSAO(GLuint depthBuffer, GLuint positionBuffer, GLuint normalBuffer);
	~SSAO();

	void CreateKernel();
	void CreateRandomTangents();

private:
	GLuint depthBuffer = 0;
	GLuint positionBuffer = 0;
	GLuint normalBuffer = 0;

	std::vector<float3> kernel;
	std::vector<std::vector<float3>> randomTangents;
};

