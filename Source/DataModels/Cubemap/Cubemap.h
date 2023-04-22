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
	
	void RenderToCubeMap(Program* usedProgram, int resolution, int roughness = 0);
	void RenderCube();

	GLuint frameBuffer;
	GLuint renderBuffer;

	GLuint cubemapTex;
	GLuint irradianceTex;
	GLuint preFilteredTex;

	unsigned int cubeVAO;
	unsigned int cubeVBO;

	std::shared_ptr<ResourceTexture> hdrTexture;
};

