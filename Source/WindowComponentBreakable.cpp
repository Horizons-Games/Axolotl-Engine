#include "WindowComponentBreakable.h"
#include "ComponentBreakable.h"

WindowComponentBreakable::WindowComponentBreakable(ComponentBreakable* component) :
	ComponentWindow("Breakable", component)
{
}

WindowComponentBreakable::~WindowComponentBreakable()
{
}

void WindowComponentBreakable::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ImGui::Text(""); //used to ignore the ImGui::SameLine called in DrawEnableAndDeleteComponent

	ComponentBreakable* componentBreakable = static_cast<ComponentBreakable*>(component);

	if (componentBreakable)
	{
		ImGui::Text("");

		bool isBreakable = componentBreakable->GetSubscribed();

		ImGui::Text("Is breakable"); ImGui::SameLine();
		if (ImGui::Checkbox("##Is breakable", &isBreakable))
		{
			if (isBreakable)
			{
				componentBreakable->SubscribeToOnCollisionEnter();
			}
			else
			{
				componentBreakable->UnsubscribeToOnCollisionEnter();
			}
		}
	}
}