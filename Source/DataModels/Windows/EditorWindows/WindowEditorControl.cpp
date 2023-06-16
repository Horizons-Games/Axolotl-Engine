#include "WindowEditorControl.h"

#include "Application.h"
#include "ModulePlayer.h"
#include "ModuleScene.h"

WindowEditorControl::WindowEditorControl() : EditorWindow("Editor Control")
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize;
}

WindowEditorControl::~WindowEditorControl()
{
}

void WindowEditorControl::DrawWindowContents()
{
	ImGuiStyle& style = ImGui::GetStyle();

	float size = ImGui::CalcTextSize("##Play").x + style.FramePadding.x * 2.0f;
	float avail = ImGui::GetContentRegionAvail().x;

	float off = (avail - size) * 0.47f;
	if (off > 0.0f)
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
	}

	Application::PlayState playState = App->GetPlayState();

	if (playState != Application::PlayState::STOPPED)
	{
		ImVec4 activeColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
		ImGui::PushStyleColor(ImGuiCol_Button, activeColor);
	}

	if (ImGui::ArrowButton("##Play", ImGuiDir_Right))
	{
		(App->GetPlayState() != Application::PlayState::STOPPED) ? App->OnStop() : App->OnPlay();
	}
	ImGui::SameLine();

	if (playState != Application::PlayState::STOPPED)
	{
		ImGui::PopStyleColor();
	}

	if (playState == Application::PlayState::PAUSED)
	{
		ImVec4 darkGray = ImVec4(.6f, .6f, .6f, 1.f);
		ImGui::PushStyleColor(ImGuiCol_Button, darkGray);
	}

	if (ImGui::Button("||"))
	{
		App->OnPause();
	}
	ImGui::SameLine();

	if (playState == Application::PlayState::PAUSED)
	{
		ImGui::PopStyleColor();
	}
}