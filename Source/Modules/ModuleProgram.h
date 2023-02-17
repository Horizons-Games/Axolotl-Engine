#pragma once
#include "Module.h"

constexpr auto ROOTPATH = "Lib/Shaders/";

class Program;

enum class ProgramType {
	MESHSHADER,
	HIGHLIGHT,
	SKYBOX
};

class ModuleProgram : public Module
{
public:
	ModuleProgram();
	~ModuleProgram() override;

	bool Start() override;

	std::shared_ptr<Program> CreateProgram(std::string vtxShaderFileName, std::string frgShaderFileName);

	bool CleanUp() override;

	void CreateProgram(unsigned vtxShader, unsigned frgShader);

	char* LoadShaderSource(const char* shaderFileName);
	unsigned CompileShader(unsigned type, const char* source);

	const unsigned& GetProgram() const { return program; }
	const std::shared_ptr<Program> GetProgram(ProgramType type) const;


private:
	unsigned program;
	std::vector<std::shared_ptr<Program> > Programs;
};

inline const std::shared_ptr<Program> ModuleProgram::GetProgram(ProgramType type) const
{
	return Programs[(int)type];
}


