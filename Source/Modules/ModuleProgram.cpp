#include "ModuleProgram.h"

#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"

#include "DataModels/Program/Program.h"
#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "GL/glew.h"

#include <fstream>

#ifdef ENGINE
const std::string ModuleProgram::rootPath = "Shaders/";
#else
const std::string ModuleProgram::rootPath = "Game/Shaders/";
#endif

ModuleProgram::ModuleProgram()
{
}

ModuleProgram::~ModuleProgram()
{
}

bool ModuleProgram::Start()
{
	std::string directory = rootPath + "Common/";
	std::vector<std::string> files = App->GetModule<ModuleFileSystem>()->ListFilesWithPath(directory.c_str());
	for (auto i = 0; i < files.size(); i++) {
		std::string path = files[i];
		if (App->GetModule<ModuleFileSystem>()->IsDirectory(path.c_str()))
		{
			path += "/";
			std::vector<std::string> filesInsideDirectory = App->GetModule<ModuleFileSystem>()->ListFilesWithPath(path.c_str());
			files.reserve(files.size() + filesInsideDirectory.size());
			files.insert(files.end(), filesInsideDirectory.begin(), filesInsideDirectory.end());
		}
		else
		{ 
			// Get the file path local to shaders/ folder:
			std::string localPath = path.substr(rootPath.size() - 1);
			// Get the absolute file path:
			path = "Shaders/" + localPath;
			// Get the file content:
			std::ifstream fileStream(path);
			std::stringstream fileBuffer;
			fileBuffer << fileStream.rdbuf();
			fileStream.close();

			// Add the file to GLSL virtual filesystem:
			glNamedStringARB(GL_SHADER_INCLUDE_ARB, -1, &(localPath.c_str()[0]), -1, fileBuffer.str().c_str());
		}
	}

	programs.reserve((int)ProgramType::PROGRAM_TYPE_SIZE);
	programs.push_back
		(CreateProgram("default_vertex.glsl", "default_fragment.glsl", "Default"));

	programs.push_back
		(CreateProgram("default_vertex.glsl", "specular_fragment.glsl", "Specular"));

	programs.push_back
		(CreateProgram("highlight_vertex.glsl", "highlight_fragment.glsl", "Highlight"));

	programs.push_back
		(CreateProgram("skybox_vertex.glsl", "skybox_fragment.glsl", "Skybox"));

	programs.push_back
		(CreateProgram("2D_vertex.glsl", "2D_fragment.glsl", "Sprite"));

	programs.push_back
		(CreateProgram("cubemap_vertex.glsl", "hdr_to_cubemap_fragment.glsl", "HDRToCubemap"));
	
	programs.push_back
		(CreateProgram("cubemap_vertex.glsl", "irradiance_cubemap_fragment.glsl", "IrradianceCubemap"));

	programs.push_back
		(CreateProgram("cubemap_vertex.glsl", "pre_filtered_map_fragment.glsl", "PreFilteredMap"));

	programs.push_back
		(CreateProgram("environment_BRDF_vertex.glsl", "environment_BRDF_fragment.glsl", "EnvironmentBRDF"));

	return true;
}

void ModuleProgram::UpdateProgram(const std::string& vtxShaderFileName, 
		const std::string& frgShaderFileName, ProgramType programType,
		const std::string& programName)
{
	std::unique_ptr<Program> program = 
		CreateProgram(vtxShaderFileName, frgShaderFileName, programName);

	if (program)
	{
		programs[static_cast<int>(programType)] = std::move(program);
	}
}

std::unique_ptr<Program> ModuleProgram::CreateProgram(const std::string& vtxShaderFileName, const std::string& frgShaderFileName,
		const std::string& programName)
{
	char* vertexBuffer{};
	App->GetModule<ModuleFileSystem>()->Load((rootPath + vtxShaderFileName).c_str(), vertexBuffer);
	unsigned vertexShader = CompileShader (GL_VERTEX_SHADER, vertexBuffer);
	delete vertexBuffer;

	char* fragmentBuffer{};
	App->GetModule<ModuleFileSystem>()->Load((rootPath + frgShaderFileName).c_str(), fragmentBuffer);
	unsigned fragmentShader = CompileShader (GL_FRAGMENT_SHADER,fragmentBuffer);
	delete fragmentBuffer;

	if (vertexShader == 0 || fragmentShader == 0)
	{
		return nullptr;
	}

	std::unique_ptr<Program> program = 
		std::make_unique<Program>(vertexShader, fragmentShader,
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

	App->GetModule<ModuleFileSystem>()->Load(shaderFileName.c_str(), data);

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
