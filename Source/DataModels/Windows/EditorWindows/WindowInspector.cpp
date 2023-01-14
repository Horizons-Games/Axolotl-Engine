#include "WindowInspector.h"

#include "imgui.h"

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleScene.h"

#include "3DModels/Model.h"
#include "GameObject/GameObject.h"
#include "Components/Component.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentLight.h"
#include "Components/ComponentBoundingBoxes.h"

WindowInspector::WindowInspector() : EditorWindow("Inspector")
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize;
}

WindowInspector::~WindowInspector()
{
}

void WindowInspector::DrawWindowContents()
{
	GameObject* currentGameObject = App->scene->GetSelectedGameObject();

	bool enable = currentGameObject->IsEnabled();
	ImGui::Checkbox("Enable", &enable);

	(enable) ? currentGameObject->Enable() : currentGameObject->Disable();

	ImGui::SameLine();

	if (currentGameObject->GetParent() == nullptr) // Keep the word Scene in the root
	{
		char* name = (char*)currentGameObject->GetName();
		if (ImGui::InputText("##GameObject", name, 24))
		{
			std::string scene = " Scene";
			std::string sceneName = name + scene;
			currentGameObject->SetName(sceneName.c_str());
		}
	}

	else
	{
		char* name = (char*)currentGameObject->GetName();
		ImGui::InputText("##GameObject", name, 24);
	}

	ImGui::Separator();

	if (WindowRightClick())
	{
		ImGui::OpenPopup("AddComponent");
	}

	if (ImGui::BeginPopup("AddComponent"))
	{
		if (App->scene->GetSelectedGameObject() != nullptr)
		{
			if (ImGui::MenuItem("Mesh Renderer"))
			{
				AddComponentMeshRenderer();
			}

			if (ImGui::MenuItem("Light"))
			{
				AddComponentLight();
			}
		}
		else
		{
			ENGINE_LOG("No GameObject is selected");
		}

		ImGui::EndPopup();
	}

	for (Component* component : currentGameObject->GetComponents())
	{
		component->Display();
	}

	//DrawTextureTable();
}

void WindowInspector::DrawTextureTable()
{
	ImGui::Text("TEXTURE");
	ImGui::Dummy(ImVec2(0.0f, 2.5f));
	if (ImGui::BeginTable("TextureTable1", 2))
	{
		ImGui::TableNextColumn();
		ImGui::Text("Height: ");
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", model.lock()->GetTextureHeight(0));
		ImGui::TableNextColumn();
		ImGui::Text("Width: ");
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", model.lock()->GetTextureWidth(0));

		ImGui::EndTable();
		ImGui::Separator();
	}

	ImGui::Image((void*)model.lock()->GetTextureID(0), ImVec2(100.0f, 100.0f), ImVec2(0, 1), ImVec2(1, 0));
}

void WindowInspector::AddComponentMeshRenderer()
{
	ComponentMeshRenderer* newMeshRenderer = (ComponentMeshRenderer*) App->scene->GetSelectedGameObject()->CreateComponent(ComponentType::MESHRENDERER);
	
	if (newMeshRenderer != nullptr)
	{
		newMeshRenderer->Init();
	}
	else
	{
		ENGINE_LOG("The Component Mesh Renderer of the selected GameObject was not created correctly");
	}
}

void WindowInspector::AddComponentLight()
{
	if (App->scene->GetSelectedGameObject()->CreateComponent(ComponentType::LIGHT) == nullptr)
	{
		ENGINE_LOG("The Component Light of the selected GameObject was not created correctly");
	}
}
