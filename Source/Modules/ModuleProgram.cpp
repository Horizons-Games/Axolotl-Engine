#include "ModuleProgram.h"

#include "DataModels/Program/Program.h"
#include "GL/glew.h"

ModuleProgram::ModuleProgram(){}
ModuleProgram::~ModuleProgram(){}


bool ModuleProgram::Start()
{
	Programs.reserve((int)ProgramType::SKYBOX + 1);
	Programs.push_back(std::unique_ptr<Program>(CreateProgram("default_vertex.glsl", "default_fragment.glsl", "Default")));
	Programs.push_back(std::unique_ptr<Program>(CreateProgram("skybox_vertex.glsl", "skybox_fragment.glsl", "Skybox")));

	return true;
}	


Program* ModuleProgram::CreateProgram(std::string vtxShaderFileName, std::string frgShaderFileName,
	std::string programName)
{
	unsigned vertexShader =
		CompileShader(GL_VERTEX_SHADER, LoadShaderSource((RootPath + vtxShaderFileName).c_str()));
	unsigned fragmentShader =
		CompileShader(GL_FRAGMENT_SHADER, LoadShaderSource((RootPath + frgShaderFileName).c_str()));

	if (vertexShader == 0 || fragmentShader == 0)
	{
		return nullptr;
	}

	Program* program = new Program(vertexShader, fragmentShader,
		vtxShaderFileName, frgShaderFileName, programName);

	if (program->GetId() == 0)
	{
		return nullptr;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

void ModuleProgram::UpdateProgram(std::string& vtxShaderFileName, std::string& frgShaderFileName, int programType,
	std::string programName)
{	
	Program* program = CreateProgram(vtxShaderFileName, frgShaderFileName, programName);

	if (program)
	{
		Programs[programType] = std::unique_ptr<Program>(program);
	}
}

bool ModuleProgram::CleanUp()
{
	return true;
}

char* ModuleProgram::LoadShaderSource(const char* shaderFileName)
{
	char* data = nullptr;
	FILE* file = nullptr;
	fopen_s(&file, shaderFileName, "rb");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		data = (char*)malloc(size + 1);
		fseek(file, 0, SEEK_SET);
		fread(data, 1, size, file);
		data[size] = 0;
		fclose(file);
	}

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
