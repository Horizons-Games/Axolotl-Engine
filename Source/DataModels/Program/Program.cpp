#include "Program.h"

#include "Globals.h"
#include "GL/glew.h"
#include <MathGeoLib/Include/Math/float3.h>

Program::Program(unsigned vertexShader, unsigned fragmentShader, 
	std::string& vtxShaderFileName, std::string& frgShaderFileName, std::string& programName) :
	vertexShaderFileName(vtxShaderFileName), fragmentShaderFileName(frgShaderFileName), programName(programName)
{
	id = glCreateProgram();
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
			char* info = (char*)malloc(len);
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

void Program::UpdateProgram(unsigned vertexShader, unsigned fragmentShader,
	std::string& vtxShaderFileName, std::string& frgShaderFileName, std::string& programName)
{
	this->programName = programName;
	vertexShaderFileName = vtxShaderFileName;
	fragmentShaderFileName = frgShaderFileName;

	CleanUp();

	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glLinkProgram(id);
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

void Program::BindUniformFloat4x4(const char* name, const float* data, bool transpose)
{
	glUniformMatrix4fv(glGetUniformLocation(id, name), 1, transpose, data);
}

void Program::BindUniformFloat3(const char* name, const float3 data)
{
	glUniform3f(glGetUniformLocation(id, name),
		data.x, data.y, data.z);
}

void Program::BindUniformFloat(const char* name, const float data)
{
	glUniform1f(glGetUniformLocation(id, name), data);
}

void Program::BindUniformInt(const char* name, int value)
{
	glUniform1i(glGetUniformLocation(id, name), value);
}

void Program::BindUniformBlock(const char* name, const unsigned value)
{
	glUniformBlockBinding(id, glGetUniformBlockIndex(id, name), value);
}

void Program::BindShaderStorageBlock(const char* name, const unsigned value)
{
	glShaderStorageBlockBinding(id, glGetProgramResourceIndex(id, GL_SHADER_STORAGE_BLOCK, name), value);
}
