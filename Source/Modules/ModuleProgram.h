#pragma once
#include "Module.h"

constexpr auto ROOTPATH = "Source/Shaders/";

class Program;

enum class ProgramType 
{
	MESHSHADER,
	HIGHLIGHT,
	SKYBOX,
	PROGRAM_TYPE_SIZE
};

class ModuleProgram : public Module
{
public:
	ModuleProgram();
	~ModuleProgram() override;

	bool Start() override;
	bool CleanUp() override;

	void UpdateProgram(std::string& vtxShaderFileName, std::string& frgShaderFileName, int programType,
		std::string programName);

	Program* GetProgram(ProgramType type) const;

private:
	std::unique_ptr<Program> CreateProgram(std::string vtxShaderFileName, std::string frgShaderFileName,
		std::string programName);

	std::string LoadShaderSource(const std::string& shaderFileName);
	unsigned CompileShader(unsigned type, const std::string& source);

	std::vector<std::unique_ptr<Program> > programs;
	std::string rootPath = "Source/Shaders/";
};

inline Program* ModuleProgram::GetProgram(ProgramType type) const
{
	if (programs.size() != 0) 
	{
		return programs[static_cast<int>(type)].get();
	}
	else
	{
		return nullptr;
	}
}
