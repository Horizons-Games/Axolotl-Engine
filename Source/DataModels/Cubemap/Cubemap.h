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

	void DebugNSight(); //DEBUG purpouse

private:
	
	void RenderToCubeMap(unsigned int cubemapTex, Program* usedProgram, int resolution, int mipmapLevel = 0);
	void CreateVAO();

	GLuint frameBuffer;
	GLuint renderBuffer;

	GLuint cubemap;
	GLuint irradiance;
	GLuint preFiltered;
	GLuint environmentBRDF;

	unsigned int cubeVAO;
	unsigned int cubeVBO;

	int numMipMaps;

	unsigned int hdrTexture;

	//std::shared_ptr<ResourceTexture> hdrTexture;
};

