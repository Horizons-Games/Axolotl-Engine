#pragma once

#include "Math/float3.h"
#include "Math/float4.h"
#include "GL/glew.h"

class Program
{
public:
	Program(unsigned vertexShader, unsigned fragmentShader,
		const std::string& vtxShaderFileName, const std::string& frgShaderFileName, const std::string& programName);
	~Program();

	void Activate();
	void Deactivate();
	bool IsValidProgram() const;

	void BindUniformFloat4x4(const std::string& name, const float* data, bool transpose);
	void BindUniformFloat4x4(const int location, const float* data, bool transpose);
	void BindUniformFloat3(const std::string& name, const float3& data);
	void BindUniformFloat3(const int location, const float3& data);
	void BindUniformFloat4(const std::string& name, const float4& data);
	void BindUniformFloat4(const int location, const float4& data);
	void BindUniformFloat(const std::string& name, const float data);
	void BindUniformFloat(const int location, const float data);
	void BindUniformInt(const std::string& name, int value);
	void BindUniformInt(const int location, int value);
	void BindUniformBlock(const std::string& name, const unsigned value);
	void BindUniformBlock(const int blockIndex, const unsigned value);
	void BindShaderStorageBlock(const std::string& name, const unsigned value);
	void BindShaderStorageBlock(const int resourceIndex, const unsigned value);

	const std::string& GetFragementShaderFileName() const;
	const std::string& GetVertexShaderFileName() const;
	const std::string& GetProgramName() const;

private:
	void CleanUp();

	unsigned id;
	std::string vertexShaderFileName;
	std::string fragmentShaderFileName;
	std::string programName;
};

inline bool Program::IsValidProgram() const
{
	return id !=0;
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

inline void Program::Activate()
{
	glUseProgram(id);
}

inline void Program::Deactivate()
{
	glUseProgram(0);
}

inline void Program::CleanUp()
{
	glDeleteProgram(id);
}

inline void Program::BindUniformFloat4x4(const std::string& name, const float* data, bool transpose)
{
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, transpose, data);
}

inline void Program::BindUniformFloat4x4(const int location, const float* data, bool transpose)
{
	glUniformMatrix4fv(location, 1, transpose, data);
}

inline void Program::BindUniformFloat3(const std::string& name, const float3& data)
{
	glUniform3f(glGetUniformLocation(id, name.c_str()),
		data.x, data.y, data.z);
}

inline void Program::BindUniformFloat3(const int location, const float3& data)
{
	glUniform3f(location, data.x, data.y, data.z);
}

inline void Program::BindUniformFloat4(const std::string& name, const float4& data)
{
	glUniform4f(glGetUniformLocation(id, name.c_str()),
		data.x, data.y, data.z, data.w);
}

inline void Program::BindUniformFloat4(const int location, const float4& data)
{
	glUniform4f(location, data.x, data.y, data.z, data.w);
}

inline void Program::BindUniformFloat(const std::string& name, const float data)
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), data);
}

inline void Program::BindUniformFloat(const int location, const float data)
{
	glUniform1f(location, data);
}

inline void Program::BindUniformInt(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

inline void Program::BindUniformInt(const int location, int value)
{
	glUniform1i(location, value);
}

inline void Program::BindUniformBlock(const std::string& name, const unsigned value)
{
	glUniformBlockBinding(id, glGetUniformBlockIndex(id, name.c_str()), value);
}

inline void Program::BindUniformBlock(const int blockIndex, const unsigned value)
{
	glUniformBlockBinding(id, blockIndex, value);
}

inline void Program::BindShaderStorageBlock(const std::string& name, const unsigned value)
{
	glShaderStorageBlockBinding(id, glGetProgramResourceIndex(id, GL_SHADER_STORAGE_BLOCK, name.c_str()), value);
}

inline void Program::BindShaderStorageBlock(const int resourceIndex, const unsigned value)
{
	glShaderStorageBlockBinding(id, resourceIndex, value);
}