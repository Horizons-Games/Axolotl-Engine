#include "WindowRenderer.h"

#include "imgui.h"

#include "Application.h"
#include "Modules/ModuleRender.h"

int WindowRenderer::bufferSize = 128;

WindowRenderer::WindowRenderer() : SubWindow("Renderer")
{
	vertexShaderBuffer = App->renderer->GetVertexShader();
	fragmentShaderBuffer = App->renderer->GetFragmentShader();
}

WindowRenderer::~WindowRenderer()
{
}

void WindowRenderer::DrawWindowContents()
{
	ImGui::TextUnformatted("Defaults shaders");
	ImGui::TextUnformatted("Vertex shader");
	ImGui::InputText("##vertex shader", &vertexShaderBuffer[0], bufferSize);
	ImGui::TextUnformatted("Fragment shader");
	ImGui::InputText("##fragment shader", &fragmentShaderBuffer[0], bufferSize);

	ImGui::Dummy(ImVec2(0.f, 5.f)); //spacing
	if (ImGui::Button("Update defaults shaders"))
	{
		App->renderer->SetShaders(vertexShaderBuffer, fragmentShaderBuffer);
	}

	ImGui::TextUnformatted("Skybox shaders");
	ImGui::TextUnformatted("Vertex shader");
	ImGui::InputText("##vertex shader", &vertexShaderBuffer[0], bufferSize);
	ImGui::TextUnformatted("Fragment shader");
	ImGui::InputText("##fragment shader", &fragmentShaderBuffer[0], bufferSize);

	ImGui::Dummy(ImVec2(0.f, 5.f)); //spacing
	if (ImGui::Button("Update default shaders"))
	{
		App->renderer->SetShaders(vertexShaderBuffer, fragmentShaderBuffer);
	}
}
