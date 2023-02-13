#pragma once
#include "Module.h"

class Program;

enum class ProgramType {
	MESHSHADER,
	SKYBOX
};

class ModuleProgram : public Module
{
public:
	ModuleProgram();
	~ModuleProgram() override;

	bool Start() override;

	std::shared_ptr<Program> CreateProgram(std::string vtxShaderFileName, std::string frgShaderFileName,
		std::string programName);

	void UpdateProgram(std::string& vtxShaderFileName, std::string& frgShaderFileName, int programType,
		std::string programName);

	bool CleanUp() override;
	
	char* LoadShaderSource(const char* shaderFileName);
	unsigned CompileShader(unsigned type, const char* source);

	const std::weak_ptr<Program> GetProgram(ProgramType type) const;

private:
	std::vector<std::shared_ptr<Program> > Programs;
	std::string RootPath = "Lib/Shaders/";
};

inline const std::weak_ptr<Program> ModuleProgram::GetProgram(ProgramType type) const
{
	return Programs[(int)type];
}




