#include "ModuleProgram.h"

#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"

#include "DataModels/Program/Program.h"
#include "GL/glew.h"

ModuleProgram::ModuleProgram() : program (0)
{
}

ModuleProgram::~ModuleProgram()
{
}

bool ModuleProgram::Start()
{
	programs.reserve((int)ProgramType::SKYBOX + 1);
	programs.push_back(CreateProgram("default_vertex.glsl", "default_fragment.glsl"));
	programs.push_back(CreateProgram("highlight_vertex.glsl", "highlight_fragment.glsl"));
	programs.push_back(CreateProgram("skybox_vertex.glsl", "skybox_fragment.glsl"));

	return true;
}


std::shared_ptr<Program> ModuleProgram::CreateProgram(std::string vtxShaderFileName, std::string frgShaderFileName)
{
	unsigned vertexShader = CompileShader(GL_VERTEX_SHADER, LoadShaderSource((ROOTPATH + vtxShaderFileName).c_str()));
	unsigned fragmentShader = CompileShader(GL_FRAGMENT_SHADER, LoadShaderSource((ROOTPATH + frgShaderFileName).c_str()));

	if (vertexShader == 0 || fragmentShader == 0)
	{
		return nullptr;
	}

	std::shared_ptr<Program> program = std::make_shared<Program>(vertexShader, fragmentShader);

	if (program->GetId() == 0)
	{
		return nullptr;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

bool ModuleProgram::CleanUp()
{
	glDeleteProgram(program);

	return true;
}

void ModuleProgram::CreateProgram(unsigned vtxShader, unsigned frgShader)
{
	program = glCreateProgram();
	glAttachShader(program, vtxShader);
	glAttachShader(program, frgShader);
	glLinkProgram(program);

	int res;
	glGetProgramiv(program, GL_LINK_STATUS, &res);
	if (res == GL_FALSE)
	{
		int len = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
		if (len > 0)
		{
			int written = 0;
			char* info = (char*)malloc(len);
			glGetProgramInfoLog(program, len, &written, info);
			ENGINE_LOG("Program Log Info: %s", info);
			free(info);
		}
	}
	glDeleteShader(vtxShader);
	glDeleteShader(frgShader);
}

char* ModuleProgram::LoadShaderSource(const char* shaderFileName)
{
	char* data;
	App->fileSystem->Load(shaderFileName, data);
	return data;
}

unsigned ModuleProgram::CompileShader(unsigned type, const char* source)
{
	unsigned shaderID = glCreateShader(type);
	glShaderSource(shaderID, 1, &source, 0);
	glCompileShader(shaderID);

	int res = GL_FALSE;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &res);
	if (res == GL_FALSE)
	{
		int len = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &len);
		if (len > 0)
		{
			int written = 0;
			char* info = (char*)malloc(len);
			glGetShaderInfoLog(shaderID, len, &written, info);
			ENGINE_LOG("Log Info: %s", info);
			free(info);
		}
	}
	return shaderID;
}
