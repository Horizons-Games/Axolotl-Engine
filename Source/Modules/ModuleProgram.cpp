#include "ModuleProgram.h"

#include "DataModels/Program/Program.h"
#include "GL/glew.h"

ModuleProgram::ModuleProgram(){}
ModuleProgram::~ModuleProgram(){}


bool ModuleProgram::Start()
{
	programs.reserve((int)ProgramType::SKYBOX + 1);
	programs.push_back(std::move(CreateProgram("default_vertex.glsl", "default_fragment.glsl")));
	programs.push_back(std::move(CreateProgram("skybox_vertex.glsl", "skybox_fragment.glsl")));

	return true;
}


std::unique_ptr<Program> ModuleProgram::CreateProgram(const std::string& vtxShaderFileName, const std::string& frgShaderFileName)
{
	unsigned vertexShader = CompileShader(GL_VERTEX_SHADER, LoadShaderSource((rootPath + vtxShaderFileName).c_str()));
	unsigned fragmentShader = CompileShader(GL_FRAGMENT_SHADER, LoadShaderSource((rootPath + frgShaderFileName).c_str()));

	if (vertexShader == 0 || fragmentShader == 0)
	{
		return nullptr;
	}

	std::unique_ptr<Program> program = std::make_unique<Program>(vertexShader, fragmentShader);

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

void ModuleProgram::CreateProgram(unsigned int vtxShader, unsigned int frgShader)
{
	this->program = glCreateProgram();
	glAttachShader(this->program, vtxShader);
	glAttachShader(this->program, frgShader);
	glLinkProgram(this->program);

	int res;
	glGetProgramiv(this->program, GL_LINK_STATUS, &res);
	if (res == GL_FALSE)
	{
		int len = 0;
		glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &len);
		if (len > 0)
		{
			int written = 0;
			char* info = (char*)malloc(len);
			glGetProgramInfoLog(this->program, len, &written, info);
			ENGINE_LOG("Program Log Info: %s", info);
			free(info);
		}
	}
	glDeleteShader(vtxShader);
	glDeleteShader(frgShader);
}

std::string ModuleProgram::LoadShaderSource(const std::string& shaderFileName)
{
	char* data = nullptr;
	FILE* file = nullptr;
	fopen_s(&file, shaderFileName.c_str(), "rb");
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
