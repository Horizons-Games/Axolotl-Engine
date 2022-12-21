#pragma once
#include "Module.h"

class ModuleProgram : public Module
{
public:
	ModuleProgram();
	~ModuleProgram() override;

	bool CleanUp() override;

	void CreateProgram(unsigned vtx_shader, unsigned frg_shader);

	char* LoadShaderSource(const char* shader_file_name);
	unsigned CompileShader(unsigned type, const char* source);

	const unsigned& GetProgram() const
	{
		return program;
	}

private:
	unsigned program;
};

