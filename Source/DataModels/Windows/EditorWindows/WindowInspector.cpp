#include "WindowInspector.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "ModuleScene.h"

#include "DataModels/Resources/Resource.h"
#include "DataModels/Resources/ResourceTexture.h"
#include "Scene/Scene.h"

#include "Components/ComponentLight.h"

#include "DataModels/Windows/SubWindows/ComponentWindows/ComponentWindow.h"

#include "Auxiliar/AddComponentAction.h"

WindowInspector::WindowInspector() :
	EditorWindow("Inspector"),
	lastSelectedObjectUID(0),
	lastSelectedGameObject(nullptr)
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize;

	auto gameObjectDoesNotHaveComponent = [](GameObject* gameObject, ComponentType componentType)
	{
		return gameObject->GetComponent(componentType) == nullptr;
	};

	actions.push_back(AddComponentAction("Create Mesh Renderer Component",
										 std::bind(&WindowInspector::AddComponentMeshRenderer, this)));

	auto isNotALight = [gameObjectDoesNotHaveComponent](GameObject* gameObject)
	{
		return gameObjectDoesNotHaveComponent(gameObject, ComponentType::LIGHT);
	};

	actions.push_back(AddComponentAction("Create Spot Light Component",
										 std::bind(&WindowInspector::AddComponentLight, this, LightType::SPOT),
										 isNotALight,
										 ComponentFunctionality::GRAPHICS));
	actions.push_back(AddComponentAction("Create Point Light Component",
										 std::bind(&WindowInspector::AddComponentLight, this, LightType::POINT),
										 isNotALight,
										 ComponentFunctionality::GRAPHICS));

	actions.push_back(AddComponentAction(
		"Create Player Component",
		std::bind(&WindowInspector::AddComponentPlayer, this),
		[gameObjectDoesNotHaveComponent](GameObject* gameObject)
		{
			return gameObjectDoesNotHaveComponent(gameObject, ComponentType::PLAYER);
		},
		ComponentFunctionality::GAMEPLAY));

	actions.push_back(AddComponentAction(
		"Create RigidBody Component",
		std::bind(&WindowInspector::AddComponentRigidBody, this),
		[gameObjectDoesNotHaveComponent](GameObject* gameObject)
		{
			return gameObjectDoesNotHaveComponent(gameObject, ComponentType::RIGIDBODY);
		},
		ComponentFunctionality::PHYSICS));

	actions.push_back(AddComponentAction(
		"Create MockState Component",
		std::bind(&WindowInspector::AddComponentMockState, this),
		[gameObjectDoesNotHaveComponent](GameObject* gameObject)
		{
			return gameObjectDoesNotHaveComponent(gameObject, ComponentType::MOCKSTATE);
		},
		ComponentFunctionality::GAMEPLAY));

	actions.push_back(AddComponentAction("Create AudioSource Component",
										 std::bind(&WindowInspector::AddComponentAudioSource, this),
										 ComponentFunctionality::AUDIO));
	actions.push_back(AddComponentAction(
		"Create AudioListener Component",
		std::bind(&WindowInspector::AddComponentAudioListener, this),
		[gameObjectDoesNotHaveComponent](GameObject* gameObject)
		{
			return gameObjectDoesNotHaveComponent(gameObject, ComponentType::AUDIOLISTENER);
		},
		ComponentFunctionality::AUDIO));

	actions.push_back(AddComponentAction(
		"Create Mesh Collider Component",
		std::bind(&WindowInspector::AddComponentMeshCollider, this),
		[gameObjectDoesNotHaveComponent](GameObject* gameObject)
		{
			return gameObjectDoesNotHaveComponent(gameObject, ComponentType::MESHCOLLIDER);
		},
		ComponentFunctionality::PHYSICS));

	actions.push_back(AddComponentAction("Create Script Component",
										 std::bind(&WindowInspector::AddComponentScript, this),
										 ComponentFunctionality::GAMEPLAY));

	actions.push_back(AddComponentAction(
		"Create Animation Component",
		std::bind(&WindowInspector::AddComponentAnimation, this),
		[gameObjectDoesNotHaveComponent](GameObject* gameObject)
		{
			return gameObjectDoesNotHaveComponent(gameObject, ComponentType::ANIMATION);
		},
		ComponentFunctionality::GAMEPLAY));

	actions.push_back(AddComponentAction("Create Breakable Component",
		std::bind(&WindowInspector::AddComponentBreakable, this),
		[gameObjectDoesNotHaveComponent](GameObject* gameObject)
		{
			return gameObjectDoesNotHaveComponent(gameObject, ComponentType::BREAKABLE);
		},
		ComponentFunctionality::PHYSICS));

	std::sort(std::begin(actions), std::end(actions));
}

WindowInspector::~WindowInspector()
{
}

void WindowInspector::DrawWindowContents()
{
	if (!resource.expired() && lastSelectedGameObject != App->GetModule<ModuleScene>()->GetSelectedGameObject())
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
	const ModuleScene* scene = App->GetModule<ModuleScene>();
	const Scene* loadedScene = scene->GetLoadedScene();

	if (lastSelectedGameObject != scene->GetSelectedGameObject())
	{
		ImGui::PushID(1);
		lastSelectedGameObject = scene->GetSelectedGameObject();
	}
	else
	{
		ImGui::PushID(0);
	}
	
	if (lastSelectedGameObject)
	{
		bool enable = lastSelectedGameObject->IsEnabled();
		ImGui::Checkbox("Enable", &enable);
		ImGui::SameLine();

		std::string name = lastSelectedGameObject->GetName();
		bool nameChanged = ImGui::InputText("##GameObject", name.data(), 32);

		if (!lastSelectedGameObject->GetParent()) // Keep the word Scene in the root
		{
			if (nameChanged)
			{
				std::string scene = " Scene";
				std::string sceneName = name.c_str() + scene;
				lastSelectedGameObject->SetName(sceneName);
			}
		}
		else
		{
			if (nameChanged)
			{
				lastSelectedGameObject->SetName(name.c_str());
			}
			ImGui::SameLine();
			bool staticness = lastSelectedGameObject->IsStatic();
			// This should be changed into a pop-up windows
			if (ImGui::Checkbox("Static", &staticness))
			{
				lastSelectedGameObject->SetStatic(staticness);
				lastSelectedGameObject->SpreadStatic();
			}

			std::string tag = lastSelectedGameObject->GetTag();
			ImGui::Text("Tag");
			ImGui::SameLine();
			if (ImGui::InputText("##Tag", tag.data(), 32))
			{
				lastSelectedGameObject->SetTag(tag.c_str());
			}
		}

		if (lastSelectedGameObject != loadedScene->GetRoot() &&
			lastSelectedGameObject != loadedScene->GetDirectionalLight())
		{
			(enable) ? lastSelectedGameObject->Enable() : lastSelectedGameObject->Disable();
		}
	}

	ImGui::Separator();

	if (WindowRightClick() && lastSelectedGameObject != loadedScene->GetRoot() &&
		lastSelectedGameObject != loadedScene->GetDirectionalLight())
	{
		ImGui::OpenPopup("AddComponent");
	}

	if (ImGui::BeginPopup("AddComponent"))
	{
		if (lastSelectedGameObject)
		{
			for (const AddComponentAction& action : actions)
			{
				if (action.condition(lastSelectedGameObject) && ImGui::MenuItem(action.actionName.c_str()))
				{
					action.callback();
				}
			}

			if (ImGui::MenuItem("Create Particle Component"))
			{
				AddComponentParticle();
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
		// if the selected game object has changed
		// or number of components is different
		// create the windows again
		if (lastSelectedGameObject->GetUID() != lastSelectedObjectUID ||
			lastSelectedGameObject->GetComponents().size() != windowsForComponentsOfSelectedObject.size())
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

	ImGui::PopID();
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

void WindowInspector::SetResource(const std::weak_ptr<Resource>& resource)
{
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
		ImGui::Image((void*) resourceTexture->GetGlTexture(), ImVec2(100, 100));
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

		const char* minFilters[] = { "NEAREST",
									 "LINEAR",
									 "NEAREST_MIPMAP_NEAREST",
									 "LINEAR_MIPMAP_NEAREST",
									 "NEAREST_MIPMAP_LINEAR",
									 "LINEAR_MIPMAP_LINEAR" };
		ImGui::Combo("MinFilter", &min, minFilters, IM_ARRAYSIZE(minFilters));

		const char* magFilters[] = { "NEAREST", "LINEAR" };
		ImGui::Combo("MagFilter", &mag, magFilters, IM_ARRAYSIZE(magFilters));

		const char* wrapFilters[] = {
			"REPEAT", "CLAMP_TO_EDGE", "CLAMP_TO_BORDER", "MIRROR_REPEAT", "MIRROR_CLAMP_TO_EDGE"
		};
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
		resourceTexture->GetLoadOptions().min = (TextureMinFilter) min;
		resourceTexture->GetLoadOptions().mag = (TextureMagFilter) mag;
		resourceTexture->GetLoadOptions().wrapS = (TextureWrap) wrapS;
		resourceTexture->GetLoadOptions().wrapT = (TextureWrap) wrapT;
		resourceTexture->Unload();
		resourceTexture->SetChanged(true);
		App->GetModule<ModuleResources>()->ReimportResource(resourceTexture->GetUID());
	}
}

void WindowInspector::AddComponentMeshRenderer()
{
	App->GetModule<ModuleScene>()->GetSelectedGameObject()->CreateComponent(ComponentType::MESHRENDERER);
}

void WindowInspector::AddComponentLight(LightType type)
{
	App->GetModule<ModuleScene>()->GetSelectedGameObject()->CreateComponentLight(type);
}

void WindowInspector::AddComponentPlayer()
{
	App->GetModule<ModuleScene>()->GetSelectedGameObject()->CreateComponent(ComponentType::PLAYER);
}

void WindowInspector::AddComponentAnimation()
{
	App->GetModule<ModuleScene>()->GetSelectedGameObject()->CreateComponent(ComponentType::ANIMATION);
}

void WindowInspector::ResetSelectedGameObject()
{
	windowsForComponentsOfSelectedObject.clear();
	lastSelectedObjectUID = 0;
}

void WindowInspector::AddComponentRigidBody()
{
	App->GetModule<ModuleScene>()->GetSelectedGameObject()->CreateComponent(ComponentType::RIGIDBODY);
}

void WindowInspector::AddComponentMockState()
{
	App->GetModule<ModuleScene>()->GetSelectedGameObject()->CreateComponent(ComponentType::MOCKSTATE);
}

void WindowInspector::AddComponentAudioSource()
{
	App->GetModule<ModuleScene>()->GetSelectedGameObject()->CreateComponent(ComponentType::AUDIOSOURCE);
}

void WindowInspector::AddComponentAudioListener()
{
	App->GetModule<ModuleScene>()->GetSelectedGameObject()->CreateComponent(ComponentType::AUDIOLISTENER);
}

void WindowInspector::AddComponentMeshCollider()
{
	App->GetModule<ModuleScene>()->GetSelectedGameObject()->CreateComponent(ComponentType::MESHCOLLIDER);
}

void WindowInspector::AddComponentScript()
{
	App->GetModule<ModuleScene>()->GetSelectedGameObject()->CreateComponent(ComponentType::SCRIPT);
}

void WindowInspector::AddComponentParticle()
{
	App->GetModule<ModuleScene>()->GetSelectedGameObject()->CreateComponent(ComponentType::PARTICLE);
}

void WindowInspector::AddComponentBreakable()
{
	App->GetModule<ModuleScene>()->GetSelectedGameObject()->CreateComponent(ComponentType::BREAKABLE);
}