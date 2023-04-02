#include "Program.h"

#include "GL/glew.h"
#include "Globals.h"
#include <MathGeoLib/Include/Math/float3.h>

Program::Program(unsigned vertexShader,
				 unsigned fragmentShader,
				 const std::string& vtxShaderFileName,
				 const std::string& frgShaderFileName,
				 const std::string& programName) :
	vertexShaderFileName(vtxShaderFileName),
	fragmentShaderFileName(frgShaderFileName),
	programName(programName),
	id(glCreateProgram())
{
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glLinkProgram(id);

	int res;
	glGetProgramiv(id, GL_LINK_STATUS, &res);
	if (res == GL_FALSE)
	{
		int len = 0;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);
		if (len > 0)
		{
			int written = 0;
			char* info = (char*) malloc(len);
			glGetProgramInfoLog(id, len, &written, info);
			ENGINE_LOG("Program Log Info: %s", info);
			free(info);
		}
		id = 0;
	}
}

Program::~Program()
{
	CleanUp();
}

void Program::Activate()
{
	glUseProgram(id);
}

void Program::Deactivate()
{
	glUseProgram(0);
}

void Program::CleanUp()
{
	glDeleteProgram(id);
}

void Program::BindUniformFloat4x4(const std::string& name, const float* data, bool transpose)
{
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, transpose, data);
}

void Program::BindUniformFloat3(const std::string& name, const float3 data)
{
	glUniform3f(glGetUniformLocation(id, name.c_str()), data.x, data.y, data.z);
}

void Program::BindUniformFloat(const std::string& name, const float data)
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), data);
}

void Program::BindUniformInt(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Program::BindUniformBlock(const std::string& name, const unsigned value)
{
	glUniformBlockBinding(id, glGetUniformBlockIndex(id, name.c_str()), value);
}

void Program::BindShaderStorageBlock(const std::string& name, const unsigned value)
{
	glShaderStorageBlockBinding(id, glGetProgramResourceIndex(id, GL_SHADER_STORAGE_BLOCK, name.c_str()), value);
}