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

	GLuint Load(const char* fileName, const char* filePath);

	int GetWidth() const;
	int GetHeight() const;

private:
	int width;
	int height;
};

