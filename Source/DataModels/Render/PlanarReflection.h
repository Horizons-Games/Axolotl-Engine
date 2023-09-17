#pragma once

#include "GL/glew.h"

class PlanarReflection
{
public:

	PlanarReflection();
	~PlanarReflection();

	void InitBuffer(unsigned width, unsigned height);

private:

	GLuint frameBuffer;
	GLuint depth;
	GLuint reflectionTex;
};

