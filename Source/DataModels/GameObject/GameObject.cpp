#include "GameObject.h"

#include "../Components/ComponentTransform.h"
#include "../Components/ComponentMeshRenderer.h"
#include "../Components/ComponentMaterial.h"
#include "../Components/ComponentCamera.h"
#include "../Components/ComponentAmbient.h"
#include "../Components/ComponentPointLight.h"
#include "../Components/ComponentDirLight.h"
#include "../Components/ComponentSpotLight.h"

#include "Application.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleDebugDraw.h"

#include "Scene/Scene.h"

#include <queue>

// Root constructor
GameObject::GameObject(const char* name) : name(name), uid(UniqueID::GenerateUID()), enabled(true),
	active(true), parent(nullptr), stateOfSelection(StateOfSelection::NO_SELECTED), 
	localAABB({ {0 ,0, 0}, {0, 0, 0} }), encapsuledAABB(localAABB), objectOBB({ localAABB }), 
	drawBoundingBoxes(false)
{
}

GameObject::GameObject(const char* name, GameObject* parent) : name(name), parent(parent),
	uid(UniqueID::GenerateUID()), enabled(true), active(true), 
	localAABB({ {0 ,0, 0}, {0, 0, 0} }), encapsuledAABB(localAABB),
	objectOBB({ localAABB }), drawBoundingBoxes(false)
{
	this->parent->AddChild(std::unique_ptr<GameObject>(this));
	active = (parent->IsEnabled() && parent->IsActive());
}

GameObject::~GameObject()
{
	// This should not be here
	std::vector<ComponentLight*> lights = GetComponentsByType<ComponentLight>(ComponentType::LIGHT);
	bool hadSpotLight = false, hadPointLight = false;
	for (ComponentLight* light : lights)
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
	//

	components.clear();

	children.clear();

	// This should not be here
	Scene* currentScene = App->scene->GetLoadedScene();

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
	//
}

void GameObject::Update()
{
	for (std::unique_ptr<Component>& component : components)
	{
		if (component->GetActive())
		{
			component->Update();
		}
	}

	//if (drawBoundingBoxes) App->debug->DrawBoundingBox(objectOBB);
}

void GameObject::Draw() const
{
#ifdef ENGINE
	if (drawBoundingBoxes)
	{
		App->debug->DrawBoundingBox(objectOBB);
	}
#endif // ENGINE
	for (const std::unique_ptr<Component>& component : components)
	{
		if (component->GetActive())
		{
			component->Draw();
		}
	}
}

void GameObject::DrawSelected()
{
	std::queue<const GameObject*> gameObjectQueue;
	gameObjectQueue.push(this);
	while (!gameObjectQueue.empty())
	{
		const GameObject* currentGo = gameObjectQueue.front();
		gameObjectQueue.pop();
		for (GameObject* child : currentGo->GetChildren())
		{
			if (child->IsEnabled())
			{
				gameObjectQueue.push(child);
			}
		}
		for (const std::unique_ptr<Component>& component : currentGo->components)
		{
			if (component->GetActive())
			{
				component->Draw();
			}
		}
#ifdef ENGINE
		if (currentGo->drawBoundingBoxes)
		{
			App->debug->DrawBoundingBox(currentGo->objectOBB);
		}

#endif // ENGINE
	}
}

void GameObject::DrawHighlight()
{
	std::queue<const GameObject*> gameObjectQueue;
	gameObjectQueue.push(this);
	while (!gameObjectQueue.empty())
	{
		const GameObject* currentGo = gameObjectQueue.front();
		gameObjectQueue.pop();
		for (GameObject* child : currentGo->GetChildren())
		{
			if (child->IsEnabled())
			{
				gameObjectQueue.push(child);
			}
		}
		std::vector<ComponentMeshRenderer*> meshes = 
			currentGo->GetComponentsByType<ComponentMeshRenderer>(ComponentType::MESHRENDERER);
		for (ComponentMeshRenderer* mesh : meshes) 
		{
			mesh->DrawHighlight();
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

void GameObject::LoadOptions(Json& meta, std::vector<GameObject*>& loadedObjects)
{
	loadedObjects.push_back(this);

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

			Component* component;
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

			GameObject* gameObject = new GameObject(name.c_str(), this);
			gameObject->LoadOptions(jsonGameObject, loadedObjects);
		}
	}
}

void GameObject::InitNewEmptyGameObject()
{
	CreateComponent(ComponentType::TRANSFORM);
}

void GameObject::SetParent(GameObject* newParent)
{
	assert(newParent);

	if (IsADescendant(newParent) ||				// Avoid dragging parent GameObjects into their descendants
		newParent->IsAChild(this))				// Avoid dragging direct children into thier parent GameObjects
	{
		return;
	}

	std::unique_ptr<GameObject> pointerToThis = parent->RemoveChild(this);
	parent = newParent;
	parent->AddChild(std::move(pointerToThis));

	(parent->IsActive() && parent->IsEnabled()) ? ActivateChildren() : DeactivateChildren();
}

void GameObject::AddChild(std::unique_ptr<GameObject> child)
{
	assert(child);

	if (!IsAChild(child.get()))
	{
		child->active = (IsActive() && IsEnabled());
		children.push_back(std::move(child));
	}
}

std::unique_ptr<GameObject> GameObject::RemoveChild(const GameObject* child)
{
	assert(child != nullptr);

	if (!IsAChild(child))
	{
		return nullptr;
	}

	for (std::vector<std::unique_ptr<GameObject>>::iterator it = children.begin();
		it != children.end(); ++it)
	{
		if ((*it).get() == child)
		{
			std::unique_ptr<GameObject> objectToBeDeleted = std::move(*it);
			children.erase(it);
			return objectToBeDeleted;
		}
	}

	return nullptr;
}

void GameObject::SetComponents(std::vector<std::unique_ptr<Component>>& components)
{
	this->components.clear();
	for (std::unique_ptr<Component>& newComponent : components)
	{
		this->components.push_back(std::move(newComponent));
	}
}

void GameObject::Enable()
{
	assert(parent != nullptr);

	enabled = true;
	active = parent->IsActive();

	for (std::unique_ptr<GameObject>& child : children)
	{
		child->ActivateChildren();
	}
}

void GameObject::Disable()
{
	assert(parent != nullptr);

	enabled = false;
	active = false;

	for (std::unique_ptr<GameObject>& child : children)
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

	for (std::unique_ptr<GameObject>& child : children)
	{
		child->DeactivateChildren();
	}
}

void GameObject::ActivateChildren()
{
	active = (parent->IsActive() && parent->IsEnabled());

	if (children.empty())
	{
		return;
	}

	for (std::unique_ptr<GameObject>& child : children)
	{
		child->ActivateChildren();
	}
}

Component* GameObject::CreateComponent(ComponentType type)
{
	std::unique_ptr<Component> newComponent;

	switch (type)
	{
		case ComponentType::TRANSFORM:
		{
			newComponent = std::make_unique<ComponentTransform>(true, this);
			break;
		}

		case ComponentType::MESHRENDERER:
		{
			newComponent = std::make_unique<ComponentMeshRenderer>(true, this);
			break;
		}
		
		case ComponentType::MATERIAL:
		{
			newComponent = std::make_unique<ComponentMaterial>(true, this);
			break;
		}

		
		case ComponentType::CAMERA:
		{
			newComponent = std::make_unique<ComponentCamera>(true, this);
			break;
		}

		case ComponentType::LIGHT:
		{
			newComponent = std::make_unique<ComponentLight>(true, this);
			break;
		}

		default:
			assert(false && "Wrong component type introduced");
	}

	if (newComponent)
	{
		Component* referenceBeforeMove = newComponent.get();
		components.push_back(std::move(newComponent));
		return referenceBeforeMove;
	}

	return nullptr;
}

Component* GameObject::CreateComponentLight(LightType lightType)
{
	std::unique_ptr<Component> newComponent;

	switch (lightType)
	{
	case LightType::AMBIENT:
		newComponent = std::make_unique<ComponentAmbient>(float3(0.05f), this);
		break;

	case LightType::DIRECTIONAL:
		newComponent = std::make_unique<ComponentDirLight>(float3(1.0f), 1.0f, this);
		break;

	case LightType::POINT:
		newComponent = std::make_unique<ComponentPointLight>(0.5f, float3(1.0f), 1.0f, this);
		break;

	case LightType::SPOT:
		newComponent = std::make_unique<ComponentSpotLight>(5.0f, 0.15f, 0.3f, float3(1.0f), 1.0f, this);
		break;
	}

	if (newComponent)
	{
		Component* referenceBeforeMove = newComponent.get();
		components.push_back(std::move(newComponent));
		return referenceBeforeMove;
	}

	return nullptr;
}

bool GameObject::RemoveComponent(const Component* component)
{
	for (std::vector<std::unique_ptr<Component>>::const_iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it).get() == component)
		{
			if ((*it)->GetType() == ComponentType::LIGHT)
			{
				ComponentLight* light = static_cast<ComponentLight*>((*it).get());

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

Component* GameObject::GetComponent(ComponentType type) const
{
	for (std::vector<std::unique_ptr<Component>>::const_iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->GetType() == type)
		{
			return (*it).get();
		}
	}
	
	return nullptr;
}

bool GameObject::IsAChild(const GameObject* child)
{
	assert(child != nullptr);

	for (std::unique_ptr<GameObject>& gameObject : children)
	{
		if (gameObject.get() == child)
		{
			return true;
		}
	}

	return false;
}

bool GameObject::IsADescendant(const GameObject* descendant)
{
	assert(descendant != nullptr);

	for (std::unique_ptr<GameObject>& child : children)
	{
		if (child.get() == descendant || child->IsADescendant(descendant))
		{
			return true;
		}
	}

	return false;
}

std::list<GameObject*> GameObject::GetGameObjectsInside()
{
	std::list<GameObject*> familyObjects = {};
	familyObjects.push_back(this);
	for (std::unique_ptr<GameObject>& children : children)
	{
		std::list<GameObject*> objectsChildren = children->GetGameObjectsInside();
		familyObjects.insert(familyObjects.end(), objectsChildren.begin(), objectsChildren.end());
	}
	return familyObjects;
}

void GameObject::MoveUpChild(GameObject* childToMove)
{
	for (std::vector<std::unique_ptr<GameObject>>::iterator it = std::begin(children);
		it != std::end(children);
		++it)
	{
		if ((*it).get() == childToMove)
		{
			std::iter_swap(it - 1, it);
			App->scene->SetSelectedGameObject((*(it - 1)).get());
			break;
		}
	}
}

void GameObject::MoveDownChild(GameObject* childToMove)
{
	for (std::vector<std::unique_ptr<GameObject>>::iterator it = std::begin(children);
		it != std::end(children);
		++it)
	{
		if ((*it).get() == childToMove)
		{
			std::iter_swap(it, it + 1);
			App->scene->SetSelectedGameObject((*(it + 1)).get());
			break;
		}
	}
}

void GameObject::CalculateBoundingBoxes()
{
	ComponentTransform* transform =
		static_cast<ComponentTransform*>(GetComponent(ComponentType::TRANSFORM));
	objectOBB = localAABB;
	objectOBB.Transform(transform->GetGlobalMatrix());
	encapsuledAABB = objectOBB.MinimalEnclosingAABB();
}

void GameObject::Encapsule(const vec* Vertices, unsigned numVertices)
{
	localAABB = localAABB.MinimalEnclosingAABB(Vertices, numVertices);
}
