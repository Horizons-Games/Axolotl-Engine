#include "ModuleProgram.h"

#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"

#include "DataModels/Program/Program.h"
#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "GL/glew.h"

ModuleProgram::ModuleProgram()
{
}

ModuleProgram::~ModuleProgram()
{
}

bool ModuleProgram::Start()
{
	programs.reserve((int)ProgramType::PROGRAM_TYPE_SIZE);
	programs.push_back(CreateProgram("default_vertex.glsl", "default_fragment.glsl", "Default"));
	programs.push_back(CreateProgram("highlight_vertex.glsl", "highlight_fragment.glsl", "Highlight"));
	programs.push_back(CreateProgram("skybox_vertex.glsl", "skybox_fragment.glsl", "Skybox"));
	programs.push_back(CreateProgram("2D_vertex.glsl", "2D_fragment.glsl", "Sprite"));


	return true;
}

void ModuleProgram::UpdateProgram(const std::string& vtxShaderFileName, const std::string& frgShaderFileName, int programType,
	const std::string programName)
{
	std::unique_ptr<Program> program = CreateProgram(vtxShaderFileName, frgShaderFileName, programName);

	if (program)
	{
		programs[programType] = std::move(program);
	}
}

std::unique_ptr<Program> ModuleProgram::CreateProgram(const std::string vtxShaderFileName, const std::string frgShaderFileName,
	const std::string programName)
{
	unsigned vertexShader =
		CompileShader(GL_VERTEX_SHADER, LoadShaderSource((rootPath + vtxShaderFileName).c_str()));
	unsigned fragmentShader =
		CompileShader(GL_FRAGMENT_SHADER, LoadShaderSource((rootPath + frgShaderFileName).c_str()));

	if (vertexShader == 0 || fragmentShader == 0)
	{
		return nullptr;
	}

	std::unique_ptr<Program> program = std::make_unique<Program>(vertexShader, fragmentShader,
		vtxShaderFileName, frgShaderFileName, programName);

	if (!program->IsValidProgram())
	{
		return nullptr;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

std::string ModuleProgram::LoadShaderSource(const std::string& shaderFileName)
{
	char* data;
	App->fileSystem->Load(shaderFileName.c_str(), data);
	return data;
}

unsigned ModuleProgram::CompileShader(unsigned type, const std::string& source)
{
	const char* sourceAsCharLValue = source.c_str();
	unsigned shaderID = glCreateShader(type);
	glShaderSource(shaderID, 1, &sourceAsCharLValue, 0);
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
