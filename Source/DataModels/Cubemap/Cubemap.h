#pragma once
#include "GL/glew.h"
#include <memory>

class ResourceCubemap;
class Program;
class Json;

class Cubemap
{
public:

	Cubemap();
	~Cubemap();
	Cubemap(std::shared_ptr<ResourceCubemap> cubemapRes);

	void SaveOptions(Json& json) const;
	void LoadOptions(Json& json);

	void DebugNSight(); //DEBUG purpouse
	
	GLuint GetIrradiance();
	GLuint GetPrefiltered();
	GLuint GetEnvironmentBRDF();
	int GetNumMiMaps();

private:
	
	void GenerateMaps();
	void RenderToCubeMap(unsigned int cubemapTex, Program* usedProgram, int resolution, int mipmapLevel = 0);
	void CreateVAO();

	GLuint frameBuffer;

	GLuint cubemap;
	GLuint irradiance;
	GLuint preFiltered;
	GLuint environmentBRDF;

	unsigned int cubeVAO;
	unsigned int cubeVBO;

	int numMipMaps;

	std::shared_ptr<ResourceCubemap> cubemapRes;
};

inline GLuint Cubemap::GetIrradiance()
{
	return irradiance;
}

inline GLuint Cubemap::GetPrefiltered()
{
	return preFiltered;
}

inline GLuint Cubemap::GetEnvironmentBRDF()
{
	return environmentBRDF;
}

inline int Cubemap::GetNumMiMaps()
{
	return numMipMaps;
}
