#pragma once
#include "GL/glew.h"
#include <memory>

class ResourceTexture;
class Program;

class Cubemap
{
public:

	Cubemap();
	~Cubemap();

private:
	
	void RenderToCubeMap(Program* usedProgram);
	void RenderCube();

	GLuint frameBuffer;
	GLuint renderBuffer;

	GLuint cubemapTex;
	GLuint irradianceTex;

	unsigned int cubeVAO;
	unsigned int cubeVBO;

	std::shared_ptr<ResourceTexture> hdrTexture;
};

