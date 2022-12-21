#pragma once
#include "Module.h"

class ModuleProgram : public Module
{
public:
	ModuleProgram();
	~ModuleProgram() override;

	bool CleanUp() override;

	void CreateProgram(unsigned vtxShader, unsigned frgShader);

	char* LoadShaderSource(const char* shaderFileName);
	unsigned CompileShader(unsigned type, const char* source);

	const unsigned& GetProgram() const
	{
		return program;
	}

private:
	unsigned program;
};

