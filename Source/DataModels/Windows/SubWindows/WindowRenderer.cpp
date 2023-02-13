#include "WindowRenderer.h"

#include <fstream>

#include "imgui.h"

#include "Application.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleProgram.h"

#include "DataModels/Program/Program.h"


int WindowRenderer::bufferSize = 128;

WindowRenderer::WindowRenderer() : SubWindow("Renderer")
{
	initialized = false;
}

WindowRenderer::~WindowRenderer()
{
}

void WindowRenderer::DrawWindowContents()
{
	if (!initialized)
	{
		vertexShaderBuffers.resize((int)ProgramType::SKYBOX + 1);
		fragmentShaderBuffer.resize((int)ProgramType::SKYBOX + 1);
		for (int i = 0; i <= (int)ProgramType::SKYBOX; i++)
		{
			std::shared_ptr<Program> program = App->program->GetProgram((ProgramType)i).lock();
			if (program)
			{
				vertexShaderBuffers[i] = program->GetVertexShaderFileName();
				fragmentShaderBuffer[i] = program->GetFragementShaderFileName();
			}
		}
		initialized = true;
	}

	//this is very hard to scale
	//everytime a new program type is added, this loop needs to be updated
	//or we must remember to always leave SKYBOX at the end of the enum
	//I think it would be better to do something similar to what was done in ModuleResources::CreateAssetAndLibFolders
	//or, as said in a comment in that function, use the library magic_enum
	for (int i = 0; i <= static_cast<int>(ProgramType::SKYBOX); i++)
	{
		std::shared_ptr<Program> program = App->program->GetProgram((ProgramType)i).lock();
		if (program)
		{
			std::string vertexShaderLabel = program->GetProgramName() + " vertex shader";
			std::string fragmentShaderLabel = program->GetProgramName() + " Fragment shader";


			ImGui::TextUnformatted(program->GetProgramName().c_str());
			ImGui::TextUnformatted("Vertex shader");
			ImGui::InputText(vertexShaderLabel.c_str(), &vertexShaderBuffers[i][0], bufferSize);
			ImGui::TextUnformatted("Fragment shader");
			ImGui::InputText(fragmentShaderLabel.c_str(), &fragmentShaderBuffer[i][0], bufferSize);

			ImGui::Dummy(ImVec2(0.f, 5.f)); //spacing

			std::string ButtonName = program->GetProgramName() + " update";

			if (ImGui::Button(ButtonName.c_str()))
			{
				std::ifstream vf(("Lib/Shaders/" + vertexShaderBuffers[i]).c_str());
				std::ifstream ff(("Lib/Shaders/" + fragmentShaderBuffer[i]).c_str());
				
				if (vf.good()
					&& ff.good())
				{
					App->program->UpdateProgram(vertexShaderBuffers[i], fragmentShaderBuffer[i],
						(ProgramType)i, program->GetProgramName());
				}
				else
				{
					vertexShaderBuffers[i] = program->GetVertexShaderFileName();
					fragmentShaderBuffer[i] = program->GetFragementShaderFileName();
				}
			}
			ImGui::Dummy(ImVec2(0.f, 10.f)); //spacing
			ImGui::Separator();
		}
	}
}
