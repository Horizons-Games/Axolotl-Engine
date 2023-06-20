#include "WindowComponentPlayer.h"

#include "Components/ComponentPlayer.h"

#include "Application.h"

WindowComponentPlayer::WindowComponentPlayer(ComponentPlayer* component) : ComponentWindow("PLAYER", component)
{
}

WindowComponentPlayer::~WindowComponentPlayer()
{
}

void WindowComponentPlayer::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
	ImGui::Text("");
	ComponentPlayer* asPlayer = static_cast<ComponentPlayer*>(component);

	if (asPlayer)
	{
		bool isStatic = asPlayer->IsStatic();

		if (ImGui::Checkbox("Static Player", &isStatic))
		{
			asPlayer->SetStatic(isStatic);
		}

		bool haveMouse = asPlayer->HaveMouseActivated();

		if (ImGui::Checkbox("Mouse when Player", &haveMouse))
		{
			asPlayer->SetMouse(haveMouse);
		}
	}
}
