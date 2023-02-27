#include "WindowInspector.h"

#include "imgui.h"

#include "Application.h"
#include "ModuleScene.h"
#include "FileSystem/ModuleResources.h"

#include "Scene/Scene.h"
#include "DataModels/Resources/Resource.h"
#include "DataModels/Resources/ResourceTexture.h"

#include "GameObject/GameObject.h"
#include "Components/Component.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentLight.h"
#include "Components/ComponentBoundingBoxes.h"

WindowInspector::WindowInspector() : EditorWindow("Inspector")
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize;
	loadScene = std::make_unique<WindowLoadScene>();
	saveScene = std::make_unique<WindowSaveScene>();
}

WindowInspector::~WindowInspector()
{
}

void WindowInspector::DrawWindowContents()
{
	if (!resource.expired() && lastSelectedGameObject != App->scene->GetSelectedGameObject().lock())
	{
		resource = std::weak_ptr<Resource>();
	}
	
	if (resource.expired())
	{
		InspectSelectedGameObject();
	}
	else
	{
		InspectSelectedResource();
	}
}

void WindowInspector::InspectSelectedGameObject()
{
	//TODO: REMOVE AFTER, HERE WE GO
	DrawButtomsSaveAndLoad();
	ImGui::Separator();
	//

	lastSelectedGameObject = App->scene->GetSelectedGameObject().lock();

	if (lastSelectedGameObject)
	{
		bool enable = lastSelectedGameObject->IsEnabled();
		ImGui::Checkbox("Enable", &enable);

		if (lastSelectedGameObject != App->scene->GetLoadedScene()->GetRoot() &&
			lastSelectedGameObject != App->scene->GetLoadedScene()->GetAmbientLight() &&
			lastSelectedGameObject != App->scene->GetLoadedScene()->GetDirectionalLight())
		{
			(enable) ? lastSelectedGameObject->Enable() : lastSelectedGameObject->Disable();
		}
	}
	else
	{
		char* name = (char*)lastSelectedGameObject->GetName();
		ImGui::InputText("##GameObject", name, 24);
	}

	if (!lastSelectedGameObject->GetParent().lock()) // Keep the word Scene in the root
	{
		char* name = (char*)lastSelectedGameObject->GetName();
		if (ImGui::InputText("##GameObject", name, 24))
		{
			std::string scene = " Scene";
			std::string sceneName = name + scene;
			lastSelectedGameObject->SetName(sceneName.c_str());
		}

	}
	else
	{
		char* name = (char*)lastSelectedGameObject->GetName();
		ImGui::InputText("##GameObject", name, 24);
	}

	ImGui::Separator();

	if (WindowRightClick() &&
		lastSelectedGameObject != App->scene->GetLoadedScene()->GetRoot() &&
		lastSelectedGameObject != App->scene->GetLoadedScene()->GetAmbientLight() &&
		lastSelectedGameObject != App->scene->GetLoadedScene()->GetDirectionalLight())
	{
		ImGui::OpenPopup("AddComponent");
	}

	if (ImGui::BeginPopup("AddComponent"))
	{
		std::shared_ptr<GameObject> go = App->scene->GetSelectedGameObject().lock();
		if (go)
		{
			if (ImGui::MenuItem("Create Mesh Renderer Component"))
			{
				AddComponentMeshRenderer();
			}

			if (!go->GetComponent(ComponentType::MATERIAL)) {
				if (ImGui::MenuItem("Create Material Component"))
				{
					AddComponentMaterial();
				}
			}

			if (!go->GetComponent(ComponentType::LIGHT)) {
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

	for (unsigned int i = 0; i < lastSelectedGameObject->GetComponents().size(); ++i)
	{
		if (lastSelectedGameObject->GetComponents()[i]->GetType() != ComponentType::TRANSFORM)
		{
			if (lastSelectedGameObject->GetComponents()[i]->GetCanBeRemoved())
			{
				DrawChangeActiveComponentContent(i, lastSelectedGameObject->GetComponents()[i]);
				ImGui::SameLine();
				if (DrawDeleteComponentContent(i, lastSelectedGameObject->GetComponents()[i]))
					break;
				ImGui::SameLine();
			}
		}

		lastSelectedGameObject->GetComponents()[i]->Display();
	}
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
		if (!App->scene->GetSelectedGameObject().lock()->RemoveComponent(component))
		{
			assert(false && "Trying to delete a non-existing component");
		}

		return true;
	}

	return false;
}

void WindowInspector::InspectSelectedResource()
{
	std::shared_ptr<Resource> resourceAsShared = resource.lock();
	if (resourceAsShared)
	{
		resourceAsShared->Load();
		//TODO When user select another resource Unload the last one

		ImGui::Text(resourceAsShared->GetFileName().c_str());
		switch (resourceAsShared->GetType())
		{
		case ResourceType::Texture:
			DrawTextureOptions();
			break;
		default:
			break;
		}
	}
}

void WindowInspector::SetResource(const std::weak_ptr<Resource>& resource) {
	std::shared_ptr<Resource> lastResource = this->resource.lock();
	/*if (lastResource) //Unload of last resource
	{
		lastResource->Unload();
	}*/

	this->resource = resource;

	std::shared_ptr<Resource> resourceAsShared = resource.lock();
	if (resourceAsShared)
	{
		switch (resourceAsShared->GetType())
		{
		case ResourceType::Texture:
			InitTextureImportOptions();
			break;
		default:
			break;
		}
	}
}

void WindowInspector::InitTextureImportOptions()
{
	std::shared_ptr<ResourceTexture> resourceTexture = std::dynamic_pointer_cast<ResourceTexture>(resource.lock());
	flipVertical = resourceTexture->GetImportOptions()->flipVertical;
	flipHorizontal = resourceTexture->GetImportOptions()->flipHorizontal;
}

void WindowInspector::DrawTextureOptions()
{
	std::shared_ptr<ResourceTexture> resourceTexture = std::dynamic_pointer_cast<ResourceTexture>(resource.lock());

	if (ImGui::BeginTable("table1", 2))
	{
		ImGui::TableNextColumn();
		ImGui::Image((void*)resourceTexture->GetGlTexture(), ImVec2(100, 100));
		ImGui::TableNextColumn();
		ImGui::Text("Width %.2f", resourceTexture->GetWidth());
		ImGui::Text("Height %.2f", resourceTexture->GetHeight());
		ImGui::EndTable();
	}
	ImGui::Text("");
	if (ImGui::CollapsingHeader("Import Options", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Flip Image Vertical", &flipVertical);
		ImGui::Checkbox("Flip Image Horizontal", &flipHorizontal);

		ImGui::Text("");
		ImGui::SameLine(ImGui::GetWindowWidth() - 110);
		if (ImGui::Button("Revert"))
		{
			InitTextureImportOptions();
		}
		ImGui::SameLine(ImGui::GetWindowWidth() - 50);
		if (ImGui::Button("Apply"))
		{
			resourceTexture->GetImportOptions()->flipVertical = flipVertical;
			resourceTexture->GetImportOptions()->flipHorizontal = flipHorizontal;
			resourceTexture->Unload();
			resourceTexture->SetChanged(true);
			App->resources->ReimportResource(resourceTexture->GetUID());
		}
	}
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Load Options", ImGuiTreeNodeFlags_DefaultOpen))
	{
		std::shared_ptr<LoadOptionsTexture> loadOptions = resourceTexture->GetLoadOptions();
		ImGui::Checkbox("MipMap", &loadOptions->mipMap);

		const char* minFilters[] = { "NEAREST", "LINEAR", "NEAREST_MIPMAP_NEAREST", "LINEAR_MIPMAP_NEAREST", "NEAREST_MIPMAP_LINEAR", "LINEAR_MIPMAP_LINEAR" };
		ImGui::Combo("MinFilter", reinterpret_cast<int*>(&loadOptions->min), minFilters, IM_ARRAYSIZE(minFilters));

		const char* magFilters[] = { "NEAREST", "LINEAR" };
		ImGui::Combo("MagFilter", reinterpret_cast<int*>(&loadOptions->mag), magFilters, IM_ARRAYSIZE(magFilters));

		const char* wrapFilters[] = { "REPEAT", "CLAMP_TO_EDGE", "CLAMP_TO_BORDER", "MIRROR_REPEAT", "MIRROR_CLAMP_TO_EDGE" };
		ImGui::Combo("WrapFilterS", reinterpret_cast<int*>(&loadOptions->wrapS), wrapFilters, IM_ARRAYSIZE(wrapFilters));
		ImGui::Combo("WrapFilterT", reinterpret_cast<int*>(&loadOptions->wrapT), wrapFilters, IM_ARRAYSIZE(wrapFilters));
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
	App->scene->GetSelectedGameObject().lock()->CreateComponent(ComponentType::MESHRENDERER);
}

void WindowInspector::AddComponentMaterial()
{
	App->scene->GetSelectedGameObject().lock()->CreateComponent(ComponentType::MATERIAL);
}

void WindowInspector::AddComponentLight(LightType type)
{
	App->scene->GetSelectedGameObject().lock()->CreateComponentLight(type);
}

// TODO: REMOVE
void WindowInspector::DrawButtomsSaveAndLoad()
{
	loadScene->DrawWindowContents();
	ImGui::SameLine();
	saveScene->DrawWindowContents();
}