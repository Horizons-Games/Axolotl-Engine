#pragma once

#include "Math/float3.h"

class Program
{

public:
	Program(unsigned vertexShader, unsigned fragmentShader, 
		std::string& vtxShaderFileName, std::string& frgShaderFileName, std::string& programName);
	~Program();
	void CleanUp();

	void UpdateProgram(unsigned vertexShader, unsigned fragmentShader,
		std::string& vtxShaderFileName, std::string& frgShaderFileName, std::string& programName);
	void Activate();
	void Deactivate();
	
	void BindUniformFloat4x4(const char* name, const float* data, bool transpose);
	void BindUniformFloat3(const char* name, const float3 data);
	void BindUniformFloat(const char* name, const float data);
	void BindUniformInt(const char* name, int value);
	void BindUniformBlock(const char* name, const unsigned value);
	void BindShaderStorageBlock(const char* name, const unsigned value);

	const unsigned& GetId() const;
	const std::string& GetFragementShaderFileName() const;
	const std::string& GetVertexShaderFileName() const;
	const std::string& GetProgramName() const;

private:
	unsigned id;
	std::string vertexShaderFileName;
	std::string fragmentShaderFileName;
	std::string programName;

};

inline const unsigned& Program::GetId() const
{
	return id;
}

inline const std::string& Program::GetFragementShaderFileName() const
{
	return fragmentShaderFileName;
}

inline const std::string& Program::GetVertexShaderFileName() const
{
	return vertexShaderFileName;
}

inline const std::string& Program::GetProgramName() const
{
	return programName;
}
