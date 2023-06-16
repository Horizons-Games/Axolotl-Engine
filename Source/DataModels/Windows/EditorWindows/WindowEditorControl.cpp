#include "WindowEditorControl.h"

#include "Application.h"
#include "ModuleInput.h"
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

	ProcessInput();
}

void WindowEditorControl::ProcessInput()
{
	if (App->GetPlayState() == Application::PlayState::STOPPED)
	{
		return;
	}

	ModuleInput* input = App->GetModule<ModuleInput>();
	if (input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::REPEAT)
	{
		if (App->GetPlayState() != Application::PlayState::STOPPED && input->GetKey(SDL_SCANCODE_Q) == KeyState::DOWN)
		{
			App->OnStop();
		}
		else if (App->GetPlayState() == Application::PlayState::RUNNING &&
				 input->GetKey(SDL_SCANCODE_A) == KeyState::DOWN)
		{
			SDL_ShowCursor(SDL_QUERY) ? input->SetShowCursor(false) : input->SetShowCursor(true);
		}
	}
}
