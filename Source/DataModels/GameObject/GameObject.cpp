#include "GameObject.h"

#include "../Components/ComponentTransform.h"
#include "../Components/ComponentMeshRenderer.h"
#include "../Components/ComponentCamera.h"
#include "../Components/ComponentLight.h"
#include "../Components/ComponentAmbient.h"
#include "../Components/ComponentPointLight.h"
#include "../Components/ComponentDirLight.h"
#include "../Components/ComponentSpotLight.h"
#include "../Components/ComponentPlayer.h"
#include "../Components/ComponentAnimation.h"
//#include "../Components/UI/ComponentBoundingBox2D.h"
#include "../Components/UI/ComponentCanvas.h"
#include "../Components/UI/ComponentImage.h"
#include "../Components/UI/ComponentButton.h"
#include "../Components/UI/ComponentTransform2D.h"
#include "../Components/ComponentRigidBody.h"
#include "../Components/ComponentMockState.h"

#include "Application.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleDebugDraw.h"

#ifndef ENGINE
#include "Modules/ModuleEditor.h"
#include "Windows/WindowDebug.h"
#endif //ENGINE

#include "Scene/Scene.h"

// Root constructor
GameObject::GameObject(const std::string& name, UID uid) : name(name), uid(uid), enabled(true),
	active(true), parent(nullptr), stateOfSelection(StateOfSelection::NO_SELECTED), staticObject(false)
{
}

GameObject::GameObject(const std::string& name) : GameObject(name, UniqueID::GenerateUID())
{
}

GameObject::GameObject(const std::string& name, GameObject* parent) : GameObject(name)
{
	this->parent = parent; //constructor using delegate constructor cannot use initializer lists
	this->parent->LinkChild(this);
	this->parentUID = parent->GetUID();
	active = (parent->IsEnabled() && parent->IsActive());
}

GameObject::GameObject(const GameObject& gameObject): name(gameObject.GetName()), parent(gameObject.GetParent()),
	uid(UniqueID::GenerateUID()), enabled(true), active(true),
	stateOfSelection(StateOfSelection::NO_SELECTED), staticObject(gameObject.staticObject)
{
	for (auto component : gameObject.GetComponents())
	{
		CopyComponent(component->GetType(), component);
	}

	for (auto child : gameObject.GetChildren())
	{
		GameObject* newChild = new GameObject(static_cast<GameObject&>(*child));
		newChild->SetParent(this);
		LinkChild(newChild);
	}
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
	for (const std::unique_ptr<Component>& component : components)
	{
		if (component->GetActive())
		{
			component->Draw();
		}
	}
}

void GameObject::SaveOptions(Json& meta)
{
	unsigned long long newParentUID = 0;
	meta["name"] = name.c_str();
	meta["tag"] = tag.c_str();
	meta["uid"] = uid;
	meta["parentUID"] = parent ? parent->GetUID() : 0;
	meta["enabled"] = (bool) enabled;
	meta["active"] = (bool) active;
	meta["static"] = (bool) staticObject;

	Json jsonComponents = meta["Components"];

	for (int i = 0; i < components.size(); ++i)
	{
		Json jsonComponent = jsonComponents[i]["Component"];

		components[i]->SaveOptions(jsonComponent);
	}	
}

void GameObject::LoadOptions(Json& meta)
{
	std::string tag = meta["tag"];
	SetTag(tag.c_str());

	staticObject = (bool)meta["static"];

	Json jsonComponents = meta["Components"];

	if(jsonComponents.Size() != 0)
	{
		for (unsigned int i = 0; i < jsonComponents.Size(); ++i)
		{
			Json jsonComponent = jsonComponents[i]["Component"];
			std::string typeName = jsonComponent["type"];

			ComponentType type = GetTypeByName(jsonComponent["type"]);
			
			if (type == ComponentType::UNKNOWN) 
				continue;
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
}

void GameObject::InitNewEmptyGameObject(bool is3D)
{
	if (is3D)
	{
		CreateComponent(ComponentType::TRANSFORM);
	}
	else 
	{
		CreateComponent(ComponentType::TRANSFORM2D);
	}
}

void GameObject::MoveParent(GameObject* newParent)
{
	assert(newParent);

	if (IsADescendant(newParent) ||				// Avoid dragging parent GameObjects into their descendants
		newParent->IsAChild(this))				// Avoid dragging direct children into thier parent GameObjects
	{
		return;
	}

	parent->UnlinkChild(this);
	newParent->LinkChild(this);

	(parent->IsActive() && parent->IsEnabled()) ? ActivateChildren() : DeactivateChildren();
}

void GameObject::LinkChild(GameObject* child)
{
	assert(child);

	if (!IsAChild(child))
	{
		child->parent = this;
		child->active = (IsActive() && IsEnabled());

		ComponentTransform* transform =
			static_cast<ComponentTransform*>(child->GetComponent(ComponentType::TRANSFORM));
		if (transform)
		{
			transform->UpdateTransformMatrices();
		}
		else
		{
			ComponentTransform2D* transform2D =
				static_cast<ComponentTransform2D*>(child->GetComponent(ComponentType::TRANSFORM2D));
			if (transform2D)
			{
				transform2D->CalculateMatrices();
			}
		}
		children.push_back(std::unique_ptr<GameObject>(child));
	}
}

GameObject* GameObject::UnlinkChild(const GameObject* child)
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
			GameObject* objectUnlinked = (*it).get();
			(*it).release();
			children.erase(it);
			return objectUnlinked;
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

void GameObject::CopyComponent(ComponentType type, Component* component)
{
	std::unique_ptr<Component> newComponent;

	switch (type)
	{
	case ComponentType::TRANSFORM:
	{
		
		newComponent = std::make_unique<ComponentTransform>(static_cast<ComponentTransform&>(*component));
		break;
	}

	case ComponentType::MESHRENDERER:
	{
		newComponent = std::make_unique<ComponentMeshRenderer>(static_cast<ComponentMeshRenderer&>(*component));
		break;
	}

	case ComponentType::CAMERA:
	{
		newComponent = std::make_unique<ComponentCamera>(static_cast<ComponentCamera&>(*component));
		break;
	}

	case ComponentType::LIGHT:
	{
		CopyComponentLight(static_cast<ComponentLight&>(*component).GetLightType(), component);
		break;
	}

	default:
		assert(false && "Wrong component type introduced");
	}

	if (newComponent)
	{
		newComponent->SetOwner(this);
		components.push_back(std::move(newComponent));
	}
}

void GameObject::CopyComponentLight(LightType type, Component* component)
{
	std::unique_ptr<ComponentLight> newComponent;

	switch (type)
	{
	case LightType::POINT:
		newComponent = std::make_unique<ComponentPointLight>(static_cast<ComponentPointLight&>(*component));
		break;

	case LightType::SPOT:
		newComponent = std::make_unique<ComponentSpotLight>(static_cast<ComponentSpotLight&>(*component));
		break;
	}

	if (newComponent)
	{
		newComponent->SetOwner(this);
		components.push_back(std::move(newComponent));
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

	for (std::unique_ptr<GameObject>& child : children)
	{
		child->DeactivateChildren();
	}
}

void GameObject::ActivateChildren()
{
	active = (parent->IsActive() && parent->IsEnabled());

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

		case ComponentType::TRANSFORM2D:
		{
			newComponent = std::make_unique<ComponentTransform2D>(true, this);
			break;
		}

		case ComponentType::MESHRENDERER:
		{
			newComponent = std::make_unique<ComponentMeshRenderer>(true, this);
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

		case ComponentType::PLAYER:
		{
			newComponent = std::make_unique<ComponentPlayer>(true, this);
			break;
		}
		
		case ComponentType::RIGIDBODY:
		{
			newComponent = std::make_unique<ComponentRigidBody>(true, this);
			break;
		}

		case ComponentType::ANIMATION:
		{
			newComponent = std::make_unique<ComponentAnimation>(true, this);
			break;
		}
		
		case ComponentType::CANVAS:
		{
			newComponent = std::make_unique<ComponentCanvas>(true, this);
			break;
		}

		case ComponentType::IMAGE:
		{
			newComponent = std::make_unique<ComponentImage>(true, this);
			break;
		}

		case ComponentType::BUTTON:
		{
			newComponent = std::make_unique<ComponentButton>(true, this);
			break;
		}

		case ComponentType::MOCKSTATE:
		{
			newComponent = std::make_unique<ComponentMockState>(true, this);
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

void GameObject::SetParentAsChildSelected()
{
	if (parent)
	{
		parent->SetStateOfSelection(StateOfSelection::CHILD_SELECTED);
		parent->SetParentAsChildSelected();
	}
}

void GameObject::SpreadStatic()
{
	for (GameObject* child : GetChildren())
	{
		child->SetStatic(staticObject);
		child->SpreadStatic();

	}
}