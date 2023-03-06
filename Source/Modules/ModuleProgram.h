#pragma once
#include "Module.h"

constexpr auto ROOTPATH = "Lib/Shaders/";

class Program;

enum class ProgramType 
{
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

	bool CleanUp() override;

	void CreateProgram(unsigned int vtxShader, unsigned int frgShader);

	std::string LoadShaderSource(const std::string& shaderFileName);
	unsigned CompileShader(unsigned type, const std::string& source);

	const unsigned GetProgram() const;
	Program* GetProgram(ProgramType type) const;

private:
	std::unique_ptr<Program> CreateProgram(const std::string& vtxShaderFileName, const std::string& frgShaderFileName);

	unsigned int program;
	std::vector<std::unique_ptr<Program> > programs;
	std::string rootPath = "Lib/Shaders/";
};

inline Program* ModuleProgram::GetProgram(ProgramType type) const
{
	return programs[static_cast<int>(type)].get();
}

inline const unsigned ModuleProgram::GetProgram() const
{
	return program;
}
