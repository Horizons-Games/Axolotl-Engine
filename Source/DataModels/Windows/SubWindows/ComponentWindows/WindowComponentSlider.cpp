#include "WindowComponentSlider.h"

#include "Components/UI/ComponentSlider.h"
#include "GameObject/GameObject.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "Application.h"

WindowComponentSlider::WindowComponentSlider(ComponentSlider* component) : ComponentWindow("SLIDER", component)
{
}

WindowComponentSlider::~WindowComponentSlider()
{
}

void WindowComponentSlider::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
	ImGui::Text("");
	ComponentSlider* asSlider = static_cast<ComponentSlider*>(component);

	if (asSlider)
	{
		GameObject* background = asSlider->GetBackground();
		if (DrawGameObject(background, "Background"))
		{
			asSlider->SetBackground(background);
			asSlider->OnHandleDragged();
		}

		GameObject* fill = asSlider->GetFill();
		if (DrawGameObject(fill, "Fill"))
		{
			asSlider->SetFill(fill);
			asSlider->OnHandleDragged();
		}

		GameObject* handle = asSlider->GetHandle();
		if (DrawGameObject(handle, "Handle"))
		{
			asSlider->SetHandle(handle);
			asSlider->OnHandleDragged();
		}

		float minValue = asSlider->GetMinValue();
		float maxValue = asSlider->GetMaxValue();
		if (ImGui::DragFloat("Range", &minValue))
		{
			if (minValue <= maxValue) asSlider->SetMinValue(minValue);
		}
		ImGui::SameLine();
		if (ImGui::DragFloat("##MaxRange", &maxValue))
		{
			if(minValue <= maxValue) asSlider->SetMaxValue(maxValue);
		}

		float currentValue = asSlider->GetCurrentValue();
		if (ImGui::SliderFloat("Value", &currentValue, minValue, maxValue))
		{
			asSlider->ModifyCurrentValue(currentValue);
		}
	}
}

bool WindowComponentSlider::DrawGameObject(GameObject*& go, const char* label)
{
	bool setGO = false;
	std::string gameObjectSlot = "Drag a GameObject here";
	if (go != nullptr)
	{
		gameObjectSlot = go->GetName().c_str();
	}

	ImGui::Button(gameObjectSlot.c_str(), ImVec2(208.0f, 20.0f));

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY"))
		{
			UID draggedGameObjectID = *(UID*) payload->Data;
			GameObject* draggedGameObject =
				App->GetModule<ModuleScene>()->GetLoadedScene()->SearchGameObjectByID(draggedGameObjectID);

			if (draggedGameObject)
			{
				go = draggedGameObject;
				setGO = true;
			}
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine(0.0f, 3.0f);

	if (go != nullptr)
	{
		if (ImGui::Button("X"))
		{
			go = nullptr;
			setGO = true;
		}
	}
	ImGui::SameLine();
	ImGui::Text(label);

	return setGO;
}
