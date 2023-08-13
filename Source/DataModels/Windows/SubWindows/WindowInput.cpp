#include "StdAfx.h"
#include "WindowInput.h"

#include "Application.h"
#include "ModuleInput.h"

namespace 
{
	const int numRows = 3;
	const int buttonsPerRow = 4;
	const int numButtons = numRows * buttonsPerRow;
	const char* buttonLabels[numButtons] = { "Q", "W", "E", "R", "A", "S", "D", "F", "Z", "X", "C", "V" };
	const int buttonScancodes[numButtons] = { SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
										  SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
										  SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V };
}

WindowInput::WindowInput() : SubWindow("Input")
{
}

WindowInput::~WindowInput()
{
}

void WindowInput::DrawWindowContents()
{
	ModuleInput* input = App->GetModule<ModuleInput>();


    ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody;
	ImGuiSelectableFlags selectableFlags = ImGuiSelectableFlags_Disabled;

	if (ImGui::BeginTable("ButtonTable", buttonsPerRow, tableFlags))
	{
		for (int i = 0; i < numButtons; ++i)
		{
			ImGui::TableNextColumn();
			ImGui::Selectable(buttonLabels[i], input->GetKey(buttonScancodes[i]) != KeyState::IDLE, selectableFlags);
		}
		ImGui::EndTable();
	}

	ImGui::Selectable("SPACE", input->GetKey(SDL_SCANCODE_SPACE) != KeyState::IDLE, selectableFlags);
}
