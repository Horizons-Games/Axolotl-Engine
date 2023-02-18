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
			Program* program = App->program->GetProgram((ProgramType)i);
			if (program)
			{
				vertexShaderBuffers[i] = program->GetVertexShaderFileName();
				fragmentShaderBuffer[i] = program->GetFragementShaderFileName();
			}
		}
		initialized = true;
	}

	for (int i = 0; i <= (int)ProgramType::SKYBOX; i++)
	{
		Program* program = App->program->GetProgram((ProgramType)i);
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
					App->program->UpdateProgram(vertexShaderBuffers[i], fragmentShaderBuffer[i], i, program->GetProgramName());
					initialized = false;
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
