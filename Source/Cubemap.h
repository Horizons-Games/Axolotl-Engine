#pragma once
#include "GL/glew.h"

class Cubemap
{
public:

	Cubemap();
	~Cubemap();

private:

	GLuint frameBuffer;
	GLuint renderBuffer;
	GLuint enviroment;
};

