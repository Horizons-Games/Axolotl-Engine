#include "GameObject.h"
#include "../Components/Component.h"
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

std::shared_ptr<GameObject> GameObject::CreateGameObject(const char* name, const std::shared_ptr<GameObject>& parent)
{
	std::shared_ptr<GameObject> newGameObject = std::make_shared<GameObject>(name);
	newGameObject->parent = parent;
	assert(newGameObject->parent);

	newGameObject->parent->AddChild(newGameObject);
	newGameObject->active = (newGameObject->parent->IsEnabled() && newGameObject->parent->IsActive());

	return newGameObject;
}

GameObject::~GameObject()
{
	components.clear();

	children.clear();
}

void GameObject::Update()
{
	for (std::shared_ptr<Component> component : components)
	{
		if (component->GetActive())
		{
			component->Update();
		}
	}
}

void GameObject::Draw()
{
	for (std::shared_ptr<Component> component : components)
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
		for (int i = 0; i < jsonComponents.Size(); ++i)
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
		for (int i = 0; i < jsonChildrens.Size(); ++i)
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

void GameObject::SetParent(const std::shared_ptr<GameObject>& newParent)
{
	assert(newParent != nullptr);

	if (this->IsADescendant(newParent) ||		// Avoid dragging parent GameObjects into their descendants
		newParent->IsAChild(shared_from_this()))				// Avoid dragging direct children into thier parent GameObjects
	{
		return;
	}

	parent->RemoveChild(shared_from_this());
	parent = newParent;
	parent->AddChild(shared_from_this());

	(parent->IsActive() && parent->IsEnabled()) ? this->ActivateChildren() : this->DeactivateChildren();
}

void GameObject::AddChild(const std::shared_ptr<GameObject>& child)
{
	assert(child != nullptr);

	if (!IsAChild(child))
	{
		children.push_back(child);
		child->active = (this->IsActive() && this->IsEnabled());
	}
}

void GameObject::AddChild(GameObject* child)
{
	this->AddChild(std::shared_ptr<GameObject>(child));
}

void GameObject::RemoveChild(const std::shared_ptr<GameObject>& child)
{
	assert(child != nullptr);

	if (!IsAChild(child))
	{
		return;
	}

	for (std::vector<std::shared_ptr<GameObject> >::const_iterator it = children.begin();
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

	for (std::shared_ptr<GameObject> child : children)
	{
		child->ActivateChildren();
	}
}

void GameObject::Disable()
{
	assert(this->parent != nullptr);

	enabled = false;
	active = false;

	for (std::shared_ptr<GameObject> child : children)
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

	for (std::shared_ptr<GameObject> child : children)
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

	for (std::shared_ptr<GameObject> child : children)
	{
		child->ActivateChildren();
	}
}

std::shared_ptr<Component> GameObject::CreateComponent(ComponentType type)
{
	std::shared_ptr<Component> newComponent = std::shared_ptr<Component>();

	switch (type)
	{
		case ComponentType::TRANSFORM:
		{
			newComponent = std::make_shared<ComponentTransform>(true, shared_from_this());
			break;
		}

		case ComponentType::MESHRENDERER:
		{
			newComponent = std::make_shared<ComponentMeshRenderer>(true, shared_from_this());
			break;
		}
		
		case ComponentType::MATERIAL:
		{
			newComponent = std::make_shared<ComponentMaterial>(true, shared_from_this());
			break;
		}

		case ComponentType::CAMERA:
		{
			newComponent = std::make_shared<ComponentCamera>(true, shared_from_this());
			break;
		}
		case ComponentType::LIGHT:
		{
			newComponent = std::make_shared<ComponentLight>(true, shared_from_this());
			break;
		}
		case ComponentType::BOUNDINGBOX:
		{
			newComponent = std::make_shared<ComponentBoundingBoxes>(true, shared_from_this());
			break;
		}

		default:
			assert(false && "Wrong component type introduced");
	}

	if (newComponent)
		components.push_back(newComponent);

	return newComponent;
}

std::shared_ptr<Component> GameObject::CreateComponentLight(LightType lightType)
{
	std::shared_ptr<Component> newComponent = std::shared_ptr<Component>();

	switch (lightType)
	{
	case LightType::AMBIENT:
		newComponent = std::make_shared<ComponentAmbient>(float3(0.05f), shared_from_this());
		break;

	case LightType::DIRECTIONAL:
		newComponent = std::make_shared<ComponentDirLight>(float3(1.0f), 1.0f, shared_from_this());
		break;

	case LightType::POINT:
		newComponent = std::make_shared<ComponentPointLight>(0.5f, float3(1.0f), 1.0f, shared_from_this());
		break;

	case LightType::SPOT:
		newComponent = std::make_shared<ComponentSpotLight>(5.0f, 0.15f, 0.3f, float3(1.0f), 1.0f, shared_from_this());
		break;
	}

	if (newComponent)
		components.push_back(newComponent);

	return newComponent;
}

bool GameObject::RemoveComponent(const std::shared_ptr<Component>& component)
{
	for (std::vector<std::shared_ptr<Component>>::const_iterator it = components.begin(); it != components.end(); ++it)
	{
		if (*it == component)
		{
			if ((*it)->GetType() == ComponentType::LIGHT)
			{
				std::shared_ptr<ComponentLight> light = std::static_pointer_cast<ComponentLight>(*it);

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

std::shared_ptr<Component> GameObject::GetComponent(ComponentType type)
{
	for (std::vector<std::shared_ptr<Component>>::const_iterator it = components.begin(); it != components.end(); ++it)
	{
		std::shared_ptr<Component> component = *it;

		if (component->GetType() == type)
		{
			return *it;
		}

	}

	return std::shared_ptr<Component>();
}

bool GameObject::IsAChild(const std::shared_ptr<GameObject>& child)
{
	assert(child != nullptr);

	for (std::shared_ptr<GameObject> gameObject : children)
	{
		if (gameObject == child)
			return true;
	}

	return false;
}

bool GameObject::IsADescendant(const std::shared_ptr<GameObject>& descendant)
{
	assert(descendant != nullptr);

	for (std::shared_ptr<GameObject> child : children)
	{
		if (child == descendant || child->IsADescendant(descendant))
			return true;
	}

	return false;
}

std::list<std::shared_ptr<GameObject> > GameObject::GetGameObjectsInside()
{

	std::list<std::shared_ptr<GameObject> > familyObjects = {};
	familyObjects.push_back(shared_from_this());
	for (std::shared_ptr<GameObject> children : this->GetChildren())
	{
		std::list<std::shared_ptr<GameObject> > objectsChildren = children->GetGameObjectsInside();
		familyObjects.insert(familyObjects.end(), objectsChildren.begin(), objectsChildren.end());
	}
	return familyObjects;
}
