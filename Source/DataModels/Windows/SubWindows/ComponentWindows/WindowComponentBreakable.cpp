#include "StdAfx.h"

#include "WindowComponentBreakable.h"
#include "Components/ComponentBreakable.h"

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
				componentBreakable->SetSubscribed(isBreakable);
			}
			else
			{
				componentBreakable->SetSubscribed(isBreakable);
			}
		}

		float impulsion = componentBreakable->GetImpulsionMul();
		ImGui::Text("");
		ImGui::Text("Impulsion"); ImGui::SameLine();
		if(ImGui::DragFloat("Impulsion power", &impulsion, 0.01f, 0.1f, 4.0f))
		{
			if (impulsion <= 4.0f)
			{
				componentBreakable->SetImpulsionMul(impulsion);
			}
			else
			{
				componentBreakable->SetImpulsionMul(1.0f);
			}
		}
	}
}