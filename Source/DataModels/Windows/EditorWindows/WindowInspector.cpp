#include "WindowInspector.h"

#include "imgui.h"

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

#include "3DModels/Model.h"
#include "GameObject/GameObject.h"
#include "Components/Component.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentMaterial.h"
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
	std::shared_ptr<GameObject> currentGameObject = App->scene->GetSelectedGameObject();

	bool enable = currentGameObject->IsEnabled();
	ImGui::Checkbox("Enable", &enable);

	if (currentGameObject != App->scene->GetLoadedScene()->GetRoot() &&
		currentGameObject != App->scene->GetLoadedScene()->GetAmbientLight() &&
		currentGameObject != App->scene->GetLoadedScene()->GetDirectionalLight())
	{
		(enable) ? currentGameObject->Enable() : currentGameObject->Disable();
	}

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

	if (WindowRightClick() && 
		currentGameObject != App->scene->GetLoadedScene()->GetRoot() &&
		currentGameObject != App->scene->GetLoadedScene()->GetAmbientLight() &&
		currentGameObject != App->scene->GetLoadedScene()->GetDirectionalLight())
	{
		ImGui::OpenPopup("AddComponent");
	}

	if (ImGui::BeginPopup("AddComponent"))
	{
		if (App->scene->GetSelectedGameObject() != nullptr)
		{
			if (ImGui::MenuItem("Create Mesh Renderer Component"))
			{
				AddComponentMeshRenderer();
			}

			if (ImGui::MenuItem("Create Material Component"))
			{
				AddComponentMaterial();
			}

			if (ImGui::MenuItem("Create Light Component"))
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
	
	for (unsigned int i = 0; i < currentGameObject->GetComponents().size(); ++i)
	{
		if (currentGameObject->GetComponents()[i]->GetType() != ComponentType::TRANSFORM)
		{
			DrawChangeActiveComponentContent(i, currentGameObject->GetComponents()[i]);
			ImGui::SameLine();

			if (currentGameObject->GetComponents()[i]->GetCanBeRemoved())
			{
				if (DrawDeleteComponentContent(i, currentGameObject->GetComponents()[i]))
					break;
				ImGui::SameLine();
			}
		}

		currentGameObject->GetComponents()[i]->Display();
	}

	//DrawTextureTable();
}

void WindowInspector::DrawChangeActiveComponentContent(int labelNum, const std::shared_ptr<Component>& component)
{
	char* textActive = new char[30];
	sprintf(textActive, "##Enabled #%d", labelNum);

	bool enable = component->GetActive();
	ImGui::Checkbox(textActive, &enable);

	(enable) ? component->Enable() : component->Disable();
}

bool WindowInspector::DrawDeleteComponentContent(int labelNum, const std::shared_ptr<Component>& component)
{
	char* textRemove = new char[30];
	sprintf(textRemove, "Remove Comp. ##%d", labelNum);

	if (ImGui::Button(textRemove, ImVec2(90, 20)))
	{
		if (!App->scene->GetSelectedGameObject()->RemoveComponent(component))
		{
			assert(false && "Trying to delete a non-existing component");
		}

		return true;
	}

	return false;
}

bool WindowInspector::MousePosIsInWindow()
{
	return (ImGui::GetIO().MousePos.x > ImGui::GetWindowPos().x
		&& ImGui::GetIO().MousePos.x < (ImGui::GetWindowPos().x + ImGui::GetWindowWidth())
		&& ImGui::GetIO().MousePos.y > ImGui::GetWindowPos().y
		&& ImGui::GetIO().MousePos.y < (ImGui::GetWindowPos().y + ImGui::GetWindowHeight()));
}

bool WindowInspector::WindowRightClick()
{
	return (ImGui::GetIO().MouseClicked[1] && MousePosIsInWindow());
}

void WindowInspector::AddComponentMeshRenderer()
{
	App->scene->GetSelectedGameObject()->CreateComponent(ComponentType::MESHRENDERER);
}

void WindowInspector::AddComponentMaterial()
{
	App->scene->GetSelectedGameObject()->CreateComponent(ComponentType::MATERIAL);
}

void WindowInspector::AddComponentLight()
{
	App->scene->GetSelectedGameObject()->CreateComponent(ComponentType::LIGHT);
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