#include "WindowDebug.h"

#include "ImGui/imgui.h"

#include "Application.h"

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
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
								ImGuiCond_Always,
								ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.25f, io.DisplaySize.y * 0.25f), ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.75f);
		if (ImGui::Begin(this->name.c_str(), NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
		{
			ImGui::TextUnformatted("cucu");
			ImGui::End();
		}
	}
}
