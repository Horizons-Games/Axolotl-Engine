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

	std::shared_ptr<Program> CreateProgram(const std::string& vtxShaderFileName, const std::string& frgShaderFileName,
		const std::string& programName);

	void UpdateProgram(const std::string& vtxShaderFileName, const std::string& frgShaderFileName,
		ProgramType programType, const std::string& programName);

	bool CleanUp() override;
	
	char* LoadShaderSource(const char* shaderFileName);
	unsigned CompileShader(unsigned type, const char* source);

	const std::weak_ptr<Program> GetProgram(ProgramType type) const;

private:
	std::vector<std::shared_ptr<Program> > programs;
	std::string rootPath = "Lib/Shaders/";
};

inline const std::weak_ptr<Program> ModuleProgram::GetProgram(ProgramType type) const
{
	return programs[(int)type];
}
