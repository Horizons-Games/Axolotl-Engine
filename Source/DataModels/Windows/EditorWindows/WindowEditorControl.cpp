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

	if (ImGui::ArrowButton("##Play", ImGuiDir_Right))
	{
		(App->IsOnPlayMode()) ? App->OnStop() : App->OnPlay();
	}
	ImGui::SameLine();

	if (ImGui::Button("||"))
	{
		App->OnPause();
	}
	ImGui::SameLine();
}