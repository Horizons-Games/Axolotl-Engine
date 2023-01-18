#pragma once
#include "Module.h"

#include "GL/glew.h"

class ModuleTexture : public Module
{
public:
	ModuleTexture();
	~ModuleTexture() override;

	bool Init();
	bool Start();

	update_status Update();

	GLuint Load(const char* fileName, const char* filePath, unsigned int & textureWidth, unsigned int & textureHeight);
	GLuint LoadCubeMap(std::vector<std::string> faces);
};

