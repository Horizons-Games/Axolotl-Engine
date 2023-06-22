#include "StdAfx.h"

#include "WindowDebug.h"

#include "ImGui/imgui.h"

#include "Application.h"
#include "GL/glew.h"

bool WindowDebug::defaultEnabled = true;

WindowDebug::WindowDebug() : Window("DebugOptions")
{
}

WindowDebug::~WindowDebug()
{
}

void WindowDebug::Draw(bool& enabled)
{
	if (App->IsDebuggingGame())
	{
		// Activating the cursor like this for now
		SDL_ShowCursor(SDL_ENABLE);
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(
			ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.25f, io.DisplaySize.y * 0.25f), ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.75f);
		if (ImGui::Begin(this->name.c_str(), NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
		{
			if (ImGui::Checkbox("Wireframe Mode", &wireframeMode))
			{
				glPolygonMode(GL_FRONT_AND_BACK, wireframeMode ? GL_LINE : GL_FILL);
			}
			ImGui::Checkbox("Draw Bounding Boxes", &drawBoundingBoxes);
			ImGui::Checkbox("Draw Area Lights", &drawAreaLight);
			ImGui::Checkbox("Draw Spot Lights", &drawSpotLight);
			ImGui::Checkbox("Draw Directional Lights", &drawDirLight);
			ImGui::Checkbox("Draw Point Lights", &drawPointLight);
			ImGui::Checkbox("Draw Physics", &drawPhysics);

			ImGui::End();
		}
	}
}
