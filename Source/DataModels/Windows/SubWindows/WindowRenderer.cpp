#include "WindowRenderer.h"

#include "Application.h"
#include "Modules/ModuleRender.h"

int WindowRenderer::bufferSize = 128;

WindowRenderer::WindowRenderer() : SubWindow("Renderer"),
	vertexShaderBuffer(App->renderer->GetVertexShader()), fragmentShaderBuffer(App->renderer->GetFragmentShader())
{
}

WindowRenderer::~WindowRenderer()
{
}

void WindowRenderer::DrawWindowContents()
{
	ImGui::TextUnformatted("Vertex shader");
	ImGui::InputText("##vertex shader", &vertexShaderBuffer[0], bufferSize);
	ImGui::TextUnformatted("Fragment shader");
	ImGui::InputText("##fragment shader", &fragmentShaderBuffer[0], bufferSize);

	ImGui::Dummy(ImVec2(0.f, 5.f)); //spacing
	if (ImGui::Button("Update shaders"))
	{
		App->renderer->SetShaders(vertexShaderBuffer, fragmentShaderBuffer);
	}
}
