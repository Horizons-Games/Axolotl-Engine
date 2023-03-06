#include "WindowInspector.h"

#include "Application.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

#include "Components/ComponentLight.h"

#include "DataModels/Windows/SubWindows/ComponentWindows/ComponentWindow.h"

WindowInspector::WindowInspector() : EditorWindow("Inspector"), 
	showSaveScene(true), showLoadScene(true), loadScene(std::make_unique<WindowLoadScene>()),
	saveScene(std::make_unique<WindowSaveScene>()), lastSelectedObjectUID(0)
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize;
}

WindowInspector::~WindowInspector()
{
}

void WindowInspector::DrawWindowContents()
{
	DrawButtomsSaveAndLoad();
	ImGui::Separator();

	GameObject* currentGameObject = App->scene->GetSelectedGameObject();

	if (currentGameObject)
	{
		bool enable = currentGameObject->IsEnabled();
		ImGui::Checkbox("Enabled", &enable);
		ImGui::SameLine();

		if (currentGameObject != App->scene->GetLoadedScene()->GetRoot() &&
			currentGameObject != App->scene->GetLoadedScene()->GetAmbientLight() &&
			currentGameObject != App->scene->GetLoadedScene()->GetDirectionalLight())
		{
			(enable) ? currentGameObject->Enable() : currentGameObject->Disable();
		}

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
		if (currentGameObject)
		{
			if (ImGui::MenuItem("Create Mesh Renderer Component"))
			{
				AddComponentMeshRenderer();
			}

			if (!currentGameObject->GetComponent(ComponentType::MATERIAL)) {
				if (ImGui::MenuItem("Create Material Component"))
				{
					AddComponentMaterial();
				}
			}

			if (!currentGameObject->GetComponent(ComponentType::LIGHT)) {
				if (ImGui::MenuItem("Create Spot Light Component"))
				{
					AddComponentLight(LightType::SPOT);
				}

				if (ImGui::MenuItem("Create Point Light Component"))
				{
					AddComponentLight(LightType::POINT);
				}
			}
			
		}

		else
		{
			ENGINE_LOG("No GameObject is selected");
		}

		ImGui::EndPopup();
	}

	if (currentGameObject)
	{
		//if the selected game object has changed
		//or number of components is different
		//create the windows again
		if (currentGameObject->GetUID() != lastSelectedObjectUID
			|| currentGameObject->GetComponents().size() != windowsForComponentsOfSelectedObject.size())
		{
			windowsForComponentsOfSelectedObject.clear();

			for (Component* component : currentGameObject->GetComponents())
			{
				windowsForComponentsOfSelectedObject.push_back(ComponentWindow::CreateWindowForComponent(component));
			}
		}
		for (int i = 0; i < windowsForComponentsOfSelectedObject.size(); ++i)
		{
			if (windowsForComponentsOfSelectedObject[i])
			{
				windowsForComponentsOfSelectedObject[i]->Draw();
			}
		}
		lastSelectedObjectUID = currentGameObject->GetUID();
	}
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

void WindowInspector::AddComponentLight(LightType type)
{
	App->scene->GetSelectedGameObject()->CreateComponentLight(type);
}

// TODO: REMOVE
void WindowInspector::DrawButtomsSaveAndLoad()
{
	loadScene->DrawWindowContents();
	ImGui::SameLine();
	saveScene->DrawWindowContents();
}