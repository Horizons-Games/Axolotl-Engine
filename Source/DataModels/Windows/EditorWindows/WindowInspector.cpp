#pragma warning (disable: 4312)

#include "WindowInspector.h"

#include "Application.h"
#include "ModuleScene.h"
#include "FileSystem/ModuleResources.h"

#include "Scene/Scene.h"
#include "DataModels/Resources/Resource.h"
#include "DataModels/Resources/ResourceTexture.h"

#include "Components/ComponentLight.h"

#include "DataModels/Windows/SubWindows/ComponentWindows/ComponentWindow.h"

WindowInspector::WindowInspector() : EditorWindow("Inspector"), lastSelectedObjectUID(0), bbDrawn(false), lastSelectedGameObject(nullptr)
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize;
}

WindowInspector::~WindowInspector()
{
}

void WindowInspector::DrawWindowContents()
{
	if (!resource.expired() && lastSelectedGameObject != App->scene->GetSelectedGameObject())
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
	lastSelectedGameObject = App->scene->GetSelectedGameObject();

	if (lastSelectedGameObject)
	{
		bool enable = lastSelectedGameObject->IsEnabled();
		ImGui::Checkbox("Enable", &enable);
		ImGui::SameLine();

		if (!lastSelectedGameObject->GetParent()) // Keep the word Scene in the root
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
			
			char* tag = (char*)lastSelectedGameObject->GetTag();
			ImGui::Text("Tag");
			ImGui::SameLine();
			ImGui::InputText("##Tag", tag, 24);
		}

		ImGui::Checkbox("##Draw Bounding Box", &(lastSelectedGameObject->drawBoundingBoxes));
		ImGui::SameLine();
		ImGui::Text("Draw Bounding Box");

		if (lastSelectedGameObject->drawBoundingBoxes != bbDrawn)
		{
			for (GameObject* child : lastSelectedGameObject->GetChildren())
			{
				if (child->drawBoundingBoxes == bbDrawn)
				{
					child->setDrawBoundingBoxes(!bbDrawn);
				}
			}

			bbDrawn = lastSelectedGameObject->drawBoundingBoxes;
		}

		if (lastSelectedGameObject != App->scene->GetLoadedScene()->GetRoot() &&
			lastSelectedGameObject != App->scene->GetLoadedScene()->GetAmbientLight() &&
			lastSelectedGameObject != App->scene->GetLoadedScene()->GetDirectionalLight())
		{
			(enable) ? lastSelectedGameObject->Enable() : lastSelectedGameObject->Disable();
		}
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
		if (lastSelectedGameObject)
		{
			if (ImGui::MenuItem("Create Mesh Renderer Component"))
			{
				AddComponentMeshRenderer();
			}

			if (!lastSelectedGameObject->GetComponent(ComponentType::MATERIAL)) {
				if (ImGui::MenuItem("Create Material Component"))
				{
					AddComponentMaterial();
				}
			}

			if (!lastSelectedGameObject->GetComponent(ComponentType::LIGHT)) {
				if (ImGui::MenuItem("Create Spot Light Component"))
				{
					AddComponentLight(LightType::SPOT);
				}

				if (ImGui::MenuItem("Create Point Light Component"))
				{
					AddComponentLight(LightType::POINT);
				}
			}

			if (!lastSelectedGameObject->GetComponent(ComponentType::PLAYER)) {
				if (ImGui::MenuItem("Create Player Component"))
				{
					AddComponentPlayer();
				}
			}

			if (!lastSelectedGameObject->GetComponent(ComponentType::RIGIDBODY)) {
				if (ImGui::MenuItem("Create RigidBody Component"))
				{
					AddComponentRigidBody();
				}
			}

			if (!lastSelectedGameObject->GetComponent(ComponentType::MOCKSTATE)) {
				if (ImGui::MenuItem("Create MockState Component"))
				{
					AddComponentMockState();
				}
			}
			
			if (ImGui::MenuItem("Create AudioSource Component"))
			{
				AddComponentAudioSource();
			}

			if (!lastSelectedGameObject->GetComponent(ComponentType::AUDIOLISTENER)) {
				if (ImGui::MenuItem("Create AudioListener Component"))
				{
					AddComponentAudioListener();
				}
			}
			

		}

		else
		{
			ENGINE_LOG("No GameObject is selected");
		}

		ImGui::EndPopup();
	}

	if (lastSelectedGameObject)
	{
		//if the selected game object has changed
		//or number of components is different
		//create the windows again
		if (lastSelectedGameObject->GetUID() != lastSelectedObjectUID
			|| lastSelectedGameObject->GetComponents().size() != windowsForComponentsOfSelectedObject.size())
		{
			windowsForComponentsOfSelectedObject.clear();

			for (Component* component : lastSelectedGameObject->GetComponents())
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
		lastSelectedObjectUID = lastSelectedGameObject->GetUID();
	}
}

void WindowInspector::InspectSelectedResource()
{
	std::shared_ptr<Resource> resourceAsShared = resource.lock();
	if (resourceAsShared)
	{
		resourceAsShared->Load();

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

	this->resource = resource;

	std::shared_ptr<Resource> resourceAsShared = resource.lock();
	if (resourceAsShared)
	{
		switch (resourceAsShared->GetType())
		{
		case ResourceType::Texture:
			InitTextureImportOptions();
			InitTextureLoadOptions();
			break;
		default:
			break;
		}
	}
}

void WindowInspector::InitTextureImportOptions()
{
	std::shared_ptr<ResourceTexture> resourceTexture = std::dynamic_pointer_cast<ResourceTexture>(resource.lock());
	flipVertical = resourceTexture->GetImportOptions().flipVertical;
	flipHorizontal = resourceTexture->GetImportOptions().flipHorizontal;
}

void WindowInspector::InitTextureLoadOptions()
{
	std::shared_ptr<ResourceTexture> resourceTexture = std::dynamic_pointer_cast<ResourceTexture>(resource.lock());
	mipMap = static_cast<int>(resourceTexture->GetLoadOptions().mipMap);
	min = static_cast<int>(resourceTexture->GetLoadOptions().min);
	mag = static_cast<int>(resourceTexture->GetLoadOptions().mag);
	wrapS = static_cast<int>(resourceTexture->GetLoadOptions().wrapS);
	wrapT = static_cast<int>(resourceTexture->GetLoadOptions().wrapT);
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
	}
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Load Options", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("MipMap", &mipMap);

		const char* minFilters[] = { "NEAREST", "LINEAR", "NEAREST_MIPMAP_NEAREST", "LINEAR_MIPMAP_NEAREST", "NEAREST_MIPMAP_LINEAR", "LINEAR_MIPMAP_LINEAR" };
		ImGui::Combo("MinFilter", &min, minFilters, IM_ARRAYSIZE(minFilters));

		const char* magFilters[] = { "NEAREST", "LINEAR" };
		ImGui::Combo("MagFilter", &mag, magFilters, IM_ARRAYSIZE(magFilters));

		const char* wrapFilters[] = { "REPEAT", "CLAMP_TO_EDGE", "CLAMP_TO_BORDER", "MIRROR_REPEAT", "MIRROR_CLAMP_TO_EDGE" };
		ImGui::Combo("WrapFilterS", &wrapS, wrapFilters, IM_ARRAYSIZE(wrapFilters));
		ImGui::Combo("WrapFilterT", &wrapT, wrapFilters, IM_ARRAYSIZE(wrapFilters));

		
	}
	ImGui::Separator();
	ImGui::Text("");
	ImGui::SameLine(ImGui::GetWindowWidth() - 110);
	if (ImGui::Button("Revert"))
	{
		InitTextureImportOptions();
		InitTextureLoadOptions();
	}
	ImGui::SameLine(ImGui::GetWindowWidth() - 50);
	if (ImGui::Button("Apply"))
	{
		resourceTexture->GetImportOptions().flipVertical = flipVertical;
		resourceTexture->GetImportOptions().flipHorizontal = flipHorizontal;
		resourceTexture->GetLoadOptions().mipMap = mipMap;
		resourceTexture->GetLoadOptions().min = (TextureMinFilter)min;
		resourceTexture->GetLoadOptions().mag = (TextureMagFilter)mag;
		resourceTexture->GetLoadOptions().wrapS = (TextureWrap)wrapS;
		resourceTexture->GetLoadOptions().wrapT = (TextureWrap)wrapT;
		resourceTexture->Unload();
		resourceTexture->SetChanged(true);
		App->resources->ReimportResource(resourceTexture->GetUID());
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

void WindowInspector::AddComponentPlayer()
{
	App->scene->GetSelectedGameObject()->CreateComponent(ComponentType::PLAYER);
}

void WindowInspector::ResetSelectedGameObject()
{
	windowsForComponentsOfSelectedObject.clear();
	lastSelectedObjectUID = 0;
}

void WindowInspector::AddComponentRigidBody()
{
	App->scene->GetSelectedGameObject()->CreateComponent(ComponentType::RIGIDBODY);
}

void WindowInspector::AddComponentMockState()
{
	App->scene->GetSelectedGameObject()->CreateComponent(ComponentType::MOCKSTATE);
}

void WindowInspector::AddComponentAudioSource()
{
	App->scene->GetSelectedGameObject()->CreateComponent(ComponentType::AUDIOSOURCE);
}

void WindowInspector::AddComponentAudioListener()
{
	App->scene->GetSelectedGameObject()->CreateComponent(ComponentType::AUDIOLISTENER);
}