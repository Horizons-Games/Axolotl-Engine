#include "GameObject.h"

#include "../Components/ComponentTransform.h"
#include "../Components/ComponentMeshRenderer.h"
#include "../Components/ComponentMaterial.h"
#include "../Components/ComponentLight.h"
#include "../Components/ComponentCamera.h"
#include "../Components/ComponentBoundingBoxes.h"
#include "../Components/ComponentAmbient.h"
#include "../Components/ComponentPointLight.h"
#include "../Components/ComponentDirLight.h"
#include "../Components/ComponentSpotLight.h"

#include "Application.h"

#include "Modules/ModuleScene.h"

#include "Scene/Scene.h"

#include "FileSystem/Json.h"

#include <assert.h>

GameObject::GameObject(const char* name) : name(name) // Root constructor
{
	uid = UniqueID::GenerateUID();
}

GameObject* GameObject::CreateGameObject(const char* name, GameObject* parent)
{
	GameObject* newGameObject = new GameObject(name);
	newGameObject->parent = parent;
	assert(newGameObject->parent);

	newGameObject->parent->AddChild(newGameObject);
	newGameObject->active = (newGameObject->parent->IsEnabled() && newGameObject->parent->IsActive());

	return newGameObject;
}

GameObject::~GameObject()
{
	std::vector<std::shared_ptr<ComponentLight> > lights = this->GetComponentsByType<ComponentLight>(ComponentType::LIGHT);
	bool hadSpotLight = false, hadPointLight = false;
	for (std::shared_ptr<ComponentLight> light : lights)
	{
		switch (light->GetLightType())
		{
		case LightType::SPOT:
			hadSpotLight = true;
			break;
		case LightType::POINT:
			hadPointLight = true;
			break;
		}
	}
	
	components.clear();

	children.clear();

	std::shared_ptr<Scene> currentScene = App->scene->GetLoadedScene();

	if (hadSpotLight)
	{
		currentScene->UpdateSceneSpotLights();
		currentScene->RenderSpotLights();
	}
	if (hadPointLight)
	{
		currentScene->UpdateScenePointLights();
		currentScene->RenderPointLights();
	}
}

void GameObject::Update()
{
	for (Component* component : components)
	{
		if (component->GetActive())
		{
			component->Update();
		}
	}
}

void GameObject::Draw()
{
	for (Component* component : components)
	{
		if (component->GetActive())
		{
			component->Draw();
		}
	}
}

void GameObject::SaveOptions(Json& meta)
{
	meta["name"] = name.c_str();
	meta["enabled"] = (bool) enabled;
	meta["active"] = (bool) active;

	Json jsonComponents = meta["Components"];

	for (int i = 0; i < components.size(); ++i)
	{
		Json jsonComponent = jsonComponents[i]["Component"];

		components[i]->SaveOptions(jsonComponent);
	}

	Json jsonChildrens = meta["Childrens"];

	for (int i = 0; i < children.size(); ++i)
	{
		Json jsonGameObject = jsonChildrens[i]["GameObject"];

		children[i]->SaveOptions(jsonGameObject);
	}
}

void GameObject::LoadOptions(Json& meta, std::vector<std::shared_ptr<GameObject> >& loadedObjects)
{
	loadedObjects.push_back(shared_from_this());

	uid = UniqueID::GenerateUID();
	name = meta["name"];
	enabled = (bool) meta["enabled"];
	active = (bool) meta["active"];
	
	Json jsonComponents = meta["Components"];

	if(jsonComponents.Size() != 0)
	{
		for (unsigned int i = 0; i < jsonComponents.Size(); ++i)
		{
			Json jsonComponent = jsonComponents[i]["Component"];
			std::string typeName = jsonComponent["type"];

			ComponentType type = GetTypeByName(jsonComponent["type"]);
			
			if (type == ComponentType::UNKNOWN) return;

			std::shared_ptr<Component> component;
			if (type == ComponentType::LIGHT)
			{
				LightType lightType = GetLightTypeByName(jsonComponent["lightType"]);
				component = CreateComponentLight(lightType);
			}
			else
			{
				component = CreateComponent(type);
			}

			component->LoadOptions(jsonComponent);
		}
	}

	Json jsonChildrens = meta["Childrens"];

	int size = jsonChildrens.Size();

	if (jsonChildrens.Size() != 0) 
	{
		for (unsigned int i = 0; i < jsonChildrens.Size(); ++i)
		{
			Json jsonGameObject = jsonChildrens[i]["GameObject"];
			std::string name = jsonGameObject["name"];

			std::shared_ptr<GameObject> gameObject = GameObject::CreateGameObject(name.c_str(), shared_from_this());
			gameObject->LoadOptions(jsonGameObject, loadedObjects);
		}
	}
}

void GameObject::InitNewEmptyGameObject()
{
	CreateComponent(ComponentType::TRANSFORM);
	CreateComponent(ComponentType::BOUNDINGBOX);
}

void GameObject::SetParent(GameObject* newParent)
{
	assert(newParent);

	if (this->IsADescendant(newParent) ||		// Avoid dragging parent GameObjects into their descendants
		newParent->IsAChild(this))				// Avoid dragging direct children into thier parent GameObjects
	{
		return;
	}

	parent->RemoveChild(this);
	parent = newParent;
	parent->AddChild(this);

	(parent->IsActive() && parent->IsEnabled()) ? this->ActivateChildren() : this->DeactivateChildren();
}

void GameObject::AddChild(GameObject* child)
{
	assert(child != nullptr);

	if (!IsAChild(child))
	{
		children.push_back(child);
		child->active = (this->IsActive() && this->IsEnabled());
	}
}

void GameObject::RemoveChild(GameObject* child)
{
	assert(child != nullptr);

	if (!IsAChild(child))
	{
		return;
	}

	for (std::vector<GameObject*>::const_iterator it = children.begin();
		it != children.end();
		++it)
	{
		if (*it == child)
		{
			children.erase(it);
			return;
		}
	}
}

void GameObject::Enable()
{
	assert(this->parent != nullptr);

	enabled = true;
	active = parent->IsActive();

	for (GameObject* child : children)
	{
		child->ActivateChildren();
	}
}

void GameObject::Disable()
{
	assert(this->parent != nullptr);

	enabled = false;
	active = false;

	for (GameObject* child : children)
	{
		child->DeactivateChildren();
	}
}

void GameObject::DeactivateChildren()
{
	active = false;

	if (children.empty())
	{
		return;
	}

	for (GameObject* child : children)
	{
		child->DeactivateChildren();
	}
}

void GameObject::ActivateChildren()
{
	active = (this->parent->IsActive() && this->parent->IsEnabled());

	if (children.empty())
	{
		return;
	}

	for (GameObject* child : children)
	{
		child->ActivateChildren();
	}
}

Component* GameObject::CreateComponent(ComponentType type)
{
	Component* newComponent;

	switch (type)
	{
		case ComponentType::TRANSFORM:
		{
			newComponent = new ComponentTransform(true, this);
			break;
		}

		case ComponentType::MESHRENDERER:
		{
			newComponent = new ComponentMeshRenderer(true, this);
			break;
		}
		
		case ComponentType::MATERIAL:
		{
			newComponent = new ComponentMaterial(true, this);
			break;
		}

		
		case ComponentType::CAMERA:
		{
			newComponent = new ComponentCamera(true, this);
			break;
		}

		case ComponentType::LIGHT:
		{
			newComponent = new ComponentLight(true, this);
			break;
		}

		case ComponentType::BOUNDINGBOX:
		{
			newComponent = new ComponentBoundingBoxes(true, this);
			break;
		}

		default:
			assert(false && "Wrong component type introduced");
	}

	if (newComponent)
		components.push_back(newComponent);

	return newComponent;
}

Component* GameObject::CreateComponentLight(LightType lightType)
{
	Component* newComponent;

	switch (lightType)
	{
	case LightType::AMBIENT:
		newComponent = new ComponentAmbient(float3(0.05f), this);
		break;

	case LightType::DIRECTIONAL:
		newComponent = new ComponentDirLight(float3(1.0f), 1.0f, this);
		break;

	case LightType::POINT:
		newComponent = new ComponentPointLight(0.5f, float3(1.0f), 1.0f, this);
		break;

	case LightType::SPOT:
		newComponent = new ComponentSpotLight(5.0f, 0.15f, 0.3f, float3(1.0f), 1.0f, this);
		break;
	}

	if (newComponent)
		components.push_back(newComponent);

	return newComponent;
}

bool GameObject::RemoveComponent(const Component* component)
{
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
	{
		if (*it == component)
		{
			if ((*it)->GetType() == ComponentType::LIGHT)
			{
				ComponentLight* light = static_cast<ComponentLight*>(*it);

				LightType type = light->GetLightType();

				components.erase(it);

				switch (type)
				{
				case LightType::POINT:
					App->scene->GetLoadedScene()->UpdateScenePointLights();
					App->scene->GetLoadedScene()->RenderPointLights();

					break;

				case LightType::SPOT:
					App->scene->GetLoadedScene()->UpdateSceneSpotLights();
					App->scene->GetLoadedScene()->RenderSpotLights();

					break;
				}
			}

			else
			{
				components.erase(it);
			}

			return true;
		}
	}

	return false;
}

Component* GameObject::GetComponent(ComponentType type)
{
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->GetType() == type)
		{
			return *it;
		}
	}

	return nullptr;
}

bool GameObject::IsAChild(const GameObject* child)
{
	assert(child != nullptr);

	for (GameObject* gameObject : children)
	{
		if (gameObject == child)
			return true;
	}

	return false;
}

bool GameObject::IsADescendant(const GameObject* descendant)
{
	assert(descendant != nullptr);

	for (GameObject* child : children)
	{
		if (child == descendant || child->IsADescendant(descendant))
			return true;
	}

	return false;
}

std::list<GameObject*> GameObject::GetGameObjectsInside()
{
	std::list<GameObject*> familyObjects = {};
	familyObjects.push_back(this);
	for (GameObject* children : this->children)
	{
		std::list<GameObject*> objectsChildren = children->GetGameObjectsInside();
		familyObjects.insert(familyObjects.end(), objectsChildren.begin(), objectsChildren.end());
	}
	return familyObjects;
}
