#include "Program.h"

#include "Globals.h"
#include "GL/glew.h"

Program::Program(unsigned vertexShader, unsigned fragmentShader)
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

void Program::BindUniformFloat3(const char* name, const float* data)
{
	glUniform3fv(glGetUniformLocation(id, name), 1, data);
}

void Program::BindUniformFloat(const char* name, const float* data)
{
	glUniform1fv(glGetUniformLocation(id, name), 1, data);
}

void Program::BindUniformBool(const char* name, bool value)
{
	glUniform1i(glGetUniformLocation(id, name), value);
}