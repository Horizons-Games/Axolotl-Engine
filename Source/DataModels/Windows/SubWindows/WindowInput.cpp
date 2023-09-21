#include "StdAfx.h"
#include "WindowInput.h"

#include "Application.h"
#include "ModuleInput.h"

namespace
{
const int numRows = 4;
const int keysPerRow = 4;
const int numKeys = numRows * keysPerRow;
const char* keyLabels[numKeys] = { "ESCAPE",
								   "TAB",
								   "SHIFT",
								   "CTRL",
								   "Q",
								   "W",
								   "E",
								   "R",
								   "A",
								   "S",
								   "D",
								   "F",
								   "Z",
								   "X",
								   "C",
								   "V" };
const int keyScancodes[numKeys] = {
	SDL_SCANCODE_ESCAPE, SDL_SCANCODE_TAB, SDL_SCANCODE_LSHIFT, SDL_SCANCODE_LCTRL, SDL_SCANCODE_Q, SDL_SCANCODE_W,
	SDL_SCANCODE_E, SDL_SCANCODE_R, SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
	SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V
};

const int numMouseButtons = 3;
const char* mouseButtonLabels[numMouseButtons] = { "MOUSE LEFT", "MOUSE MIDDLE", "MOUSE RIGHT" };
const int mouseButtonScancodes[numMouseButtons] = { SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE, SDL_BUTTON_RIGHT };
} // namespace

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

	if (ImGui::BeginTable("KeyTable", keysPerRow, tableFlags))
	{
		for (int i = 0; i < numKeys; ++i)
		{
			ImGui::TableNextColumn();
			ImGui::Selectable(keyLabels[i], input->GetKey(keyScancodes[i]) != KeyState::IDLE, selectableFlags);
		}
		ImGui::EndTable();
	}

	ImGui::Selectable("SPACE", input->GetKey(SDL_SCANCODE_SPACE) != KeyState::IDLE, selectableFlags);

	if (ImGui::BeginTable("MouseButtonTable", numMouseButtons, tableFlags))
	{
		for (int i = 0; i < numMouseButtons; ++i)
		{
			ImGui::TableNextColumn();
			ImGui::Selectable(mouseButtonLabels[i],
							  input->GetMouseButton(mouseButtonScancodes[i]) != KeyState::IDLE,
							  selectableFlags);
		}
		ImGui::EndTable();
	}
}
