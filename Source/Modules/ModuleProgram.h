#pragma once
#include "Module.h"

class Program;

enum class ProgramType
{
	DEFAULT,
	SPECULAR,
	HIGHLIGHT,
	SKYBOX,
	SPRITE,
	HDR_TO_CUBEMAP,
	IRRADIANCE_MAP,
	PRE_FILTERED_MAP,
	ENVIRONMENT_BRDF,
	PARTICLES,
	DEFERRED_LIGHT,
	G_METALLIC,
	G_SPECULAR,
	SHADOW_MAPPING,
	PARALLEL_REDUCTION,
	MIN_MAX,
	SHADOW_DEPTH_VARIANCE,
	GAUSSIAN_BLUR,
	//BLOOM,
	KAWASE_DOWN,
	KAWASE_UP,
	SSAO,
	COMPONENT_LINE,
	COLOR_CORRECTION,
	TRAIL,
	PROGRAM_TYPE_SIZE
};

class ModuleProgram : public Module
{
public:
	ModuleProgram();
	~ModuleProgram() override;

	bool Start() override;

	void UpdateProgram(const std::string& vtxShaderFileName,
					   const std::string& frgShaderFileName,
					   ProgramType programType,
					   const std::string& programName);

	Program* GetProgram(ProgramType type) const;

private:
	std::unique_ptr<Program> CreateProgram(const std::string& vtxShaderFileName,
										   const std::string& frgShaderFileName,
										   const std::string& gtyShaderFileName,
										   const std::string& programName);
	std::unique_ptr<Program> CreateProgram(const std::string& vtxShaderFileName,
										   const std::string& frgShaderFileName,
										   const std::string& programName);
	std::unique_ptr<Program> CreateProgram(const std::string& computeShaderName,
										   const std::string& programName);

	std::string LoadShaderSource(const std::string& shaderFileName);
	unsigned CompileShader(unsigned type, const std::string& source);

	std::vector<std::unique_ptr<Program>> programs;
	static const std::string rootPath;
};

inline Program* ModuleProgram::GetProgram(ProgramType type) const
{
	if (programs.empty() || programs.size() > static_cast<int>(ProgramType::PROGRAM_TYPE_SIZE))
	{
		return nullptr;
	}
	else
	{
		return programs[static_cast<int>(type)].get();
	}
}
