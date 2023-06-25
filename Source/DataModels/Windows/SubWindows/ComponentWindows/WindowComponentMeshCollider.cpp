#include "StdAfx.h"

#include "WindowComponentMeshCollider.h"

#include "Components/ComponentMeshCollider.h"

WindowComponentMeshCollider::WindowComponentMeshCollider(ComponentMeshCollider* component) :
	ComponentWindow("MESH COLLIDER", component)
{
}

WindowComponentMeshCollider::~WindowComponentMeshCollider()
{
}

void WindowComponentMeshCollider::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ImGui::Text(""); // used to ignore the ImGui::SameLine called in DrawEnableAndDeleteComponent

	ComponentMeshCollider* asMeshCollider = static_cast<ComponentMeshCollider*>(component);

	if (asMeshCollider)
	{
		ImGui::Text("");

		bool isTrigger = asMeshCollider->GetIsTrigger();

		ImGui::Text("Is Trigger");
		ImGui::SameLine();
		if (ImGui::Checkbox("##Is SetIsTrigger", &isTrigger))
		{
			asMeshCollider->SetIsTrigger(isTrigger);
		}
	}
}
