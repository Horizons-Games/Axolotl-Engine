#include "ModuleProgram.h"

#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"

#include "DataModels/Program/Program.h"
#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "GL/glew.h"

#include <fstream>

ModuleProgram::ModuleProgram()
{
}

ModuleProgram::~ModuleProgram()
{
}

bool ModuleProgram::Start()
{
	std::string directory = rootPath + "Common/";
	std::vector<std::string> files = App->fileSystem->ListFilesWithPath(directory.c_str());
	for (auto i = 0; i < files.size(); i++) {
		std::string path = files[i];
		if (App->fileSystem->IsDirectory(path.c_str())) 
		{
			path += "/";
			std::vector<std::string> filesInsideDirectory = App->fileSystem->ListFilesWithPath(path.c_str());
			files.reserve(files.size() + filesInsideDirectory.size());
			files.insert(files.end(), filesInsideDirectory.begin(), filesInsideDirectory.end());
		}
		else
		{ 
			// Get the file path local to shaders/ folder:
			std::string localPath = path.substr(rootPath.size());
			// Get the absolute file path:

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

	return true;
}

void ModuleProgram::UpdateProgram
	(const std::string& vtxShaderFileName, 
		const std::string& frgShaderFileName, int programType,
		const std::string programName)
{
	std::unique_ptr<Program> program = 
		CreateProgram(vtxShaderFileName, frgShaderFileName, programName);

	if (program)
	{
		programs[programType] = std::move(program);
	}
}

std::unique_ptr<Program> ModuleProgram::CreateProgram
	(const std::string vtxShaderFileName, const std::string frgShaderFileName,
		const std::string programName)
{
	unsigned vertexShader =
		CompileShader
			(GL_VERTEX_SHADER, 
				LoadShaderSource((rootPath + vtxShaderFileName).c_str()));

	unsigned fragmentShader =
		CompileShader
			(GL_FRAGMENT_SHADER, 
				LoadShaderSource((rootPath + frgShaderFileName).c_str()));

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

bool ModuleProgram::CleanUp()
{
	return true;
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
