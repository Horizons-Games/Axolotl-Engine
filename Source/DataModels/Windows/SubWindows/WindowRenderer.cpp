#include "WindowRenderer.h"

#include "imgui.h"

#include "Application.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleProgram.h"

#include "DataModels/Program/Program.h"

int WindowRenderer::bufferSize = 128;

WindowRenderer::WindowRenderer() : SubWindow("Renderer")
{
}

WindowRenderer::~WindowRenderer()
{
}

void WindowRenderer::DrawWindowContents()
{

	for (int i = 0; i <= (int)ProgramType::SKYBOX;i++) 
	{
		std::shared_ptr<Program> program = App->program->GetProgram((ProgramType)i).lock();
		if (program)
		{
			std::string vertexShaderBuffer = program->GetVertexShaderFileName();
			std::string fragmentShaderBuffer = program->GetFragementShaderFileName();
			ImGui::TextUnformatted(program->GetProgramName().c_str());
			ImGui::TextUnformatted("Vertex shader");
			ImGui::InputText("##vertex shader", &vertexShaderBuffer[0], bufferSize);
			ImGui::TextUnformatted("Fragment shader");
			ImGui::InputText("##fragment shader", &fragmentShaderBuffer[0], bufferSize);

			ImGui::Dummy(ImVec2(0.f, 5.f)); //spacing

			if (ImGui::Button( "Update shaders"))
			{
				App->renderer->SetShaders(vertexShaderBuffer, fragmentShaderBuffer);
			}
			ImGui::Dummy(ImVec2(0.f, 10.f)); //spacing
			ImGui::Separator();
		}
	}
}
