#include "GameObject.h"

#include "../Components/ComponentAnimation.h"
#include "../Components/ComponentAudioListener.h"
#include "../Components/ComponentAudioSource.h"
#include "../Components/ComponentBreakable.h"
#include "../Components/ComponentCamera.h"
#include "../Components/ComponentCubemap.h"
#include "../Components/ComponentDirLight.h"
#include "../Components/ComponentLight.h"
#include "../Components/ComponentMeshCollider.h"
#include "../Components/ComponentMeshRenderer.h"
#include "../Components/ComponentMockState.h"
#include "../Components/ComponentPlayer.h"
#include "../Components/ComponentPointLight.h"
#include "../Components/ComponentRigidBody.h"
#include "../Components/ComponentScript.h"
#include "../Components/ComponentSpotLight.h"
#include "../Components/ComponentTransform.h"
#include "../Components/UI/ComponentButton.h"
#include "../Components/UI/ComponentCanvas.h"
#include "../Components/UI/ComponentImage.h"
#include "../Components/UI/ComponentTransform2D.h"

#include "Application.h"

#include "Modules/ModuleDebugDraw.h"
#include "Modules/ModuleScene.h"

#ifndef ENGINE
	#include "Modules/ModuleEditor.h"
	#include "Windows/WindowDebug.h"
#endif // ENGINE

#include "Scene/Scene.h"

// Root constructor
GameObject::GameObject(const std::string& name, UID uid) :
	GameObject(name, nullptr, uid, true, true, StateOfSelection::NO_SELECTED, false)
{
}

GameObject::GameObject(const std::string& name) : GameObject(name, UniqueID::GenerateUID())
{
}

GameObject::GameObject(const std::string& name, GameObject* parent) :
	GameObject(name,
			   parent,
			   UniqueID::GenerateUID(),
			   true,
			   (parent->IsEnabled() && parent->IsActive()),
			   StateOfSelection::NO_SELECTED,
			   false)
{
	this->parent->LinkChild(this);
}

GameObject::GameObject(const GameObject& gameObject) :
	GameObject(gameObject.name,
			   gameObject.parent,
			   UniqueID::GenerateUID(),
			   true,
			   true,
			   StateOfSelection::NO_SELECTED,
			   gameObject.staticObject,
			   gameObject.tag)
{
	for (const std::unique_ptr<Component>& component : gameObject.components)
	{
		CopyComponent(component.get());
	}

	for (const std::unique_ptr<GameObject>& child : gameObject.children)
	{
		GameObject* newChild = new GameObject(*child);
		newChild->parent = this;
		LinkChild(newChild);
	}
}

GameObject::GameObject(const std::string& name,
					   GameObject* parent,
					   UID uid,
					   bool enabled,
					   bool active,
					   StateOfSelection selection,
					   bool staticObject,
					   const std::string& tag) :
	name(name),
	parent(parent),
	uid(uid),
	enabled(enabled),
	active(active),
	stateOfSelection(selection),
	staticObject(staticObject),
	tag(tag)
{
}

GameObject::~GameObject()
{
	components.clear();
	children.clear();
}

void GameObject::Save(Json& meta)
{
	unsigned long long newParentUID = 0;
	meta["name"] = name.c_str();
	meta["tag"] = tag.c_str();
	meta["uid"] = uid;
	meta["parentUID"] = parent ? parent->GetUID() : 0;
	meta["enabled"] = (bool) enabled;
	meta["static"] = (bool) staticObject;

	Json jsonComponents = meta["Components"];

	for (int i = 0; i < components.size(); ++i)
	{
		Json jsonComponent = jsonComponents[i]["Component"];

		components[i]->Save(jsonComponent);
	}
}

void GameObject::Load(const Json& meta)
{
	std::string tag = meta["tag"];
	SetTag(tag.c_str());

	staticObject = (bool) meta["static"];

	Json jsonComponents = meta["Components"];

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
			component = CreateComponentLight(lightType, AreaType::NONE); // TODO look at this when implement metas
		}
		else
		{
			component = CreateComponent(type);
		}

		component->Load(jsonComponent);
	}
}
}

void GameObject::Draw() const
{
	for (const std::unique_ptr<Component>& component : components)
	{
		if (component->IsEnabled())
		{
			Drawable* drawable = dynamic_cast<Drawable*>(component.get());
			if (drawable)
			{
				drawable->Draw();
			}
		}
	}
}

void GameObject::InitNewEmptyGameObject(bool is3D)
{
	if (is3D)
	{
		CreateComponent<ComponentTransform>();
	}
	else
	{
		CreateComponent<ComponentTransform2D>();
	}
}

void GameObject::SetParent(GameObject* newParent)
{
	assert(newParent);

	if (IsADescendant(newParent) || // Avoid dragging parent GameObjects into their descendants
		newParent->IsAChild(this))	// Avoid dragging direct children into thier parent GameObjects
	{
		return;
	}

	// it's fine to ignore the return value in this case
	// since the pointer returned will be "this"
	std::ignore = parent->UnlinkChild(this);

	ComponentTransform* transform = this->GetComponent<ComponentTransform>();
	const ComponentTransform* newParentTransform = newParent->GetComponent<ComponentTransform>();
	if (transform && newParentTransform)
	{
		transform->CalculateLocalFromNewGlobal(newParentTransform);
	}
	newParent->LinkChild(this);

	(parent->IsActive() && parent->IsEnabled()) ? Activate() : Deactivate();
}

void GameObject::LinkChild(GameObject* child)
{
	assert(child);

	if (!IsAChild(child))
	{
		child->parent = this;
		child->active = (IsActive() && IsEnabled());

		ComponentTransform* transform = child->GetComponent<ComponentTransform>();
		if (transform)
		{
			transform->UpdateTransformMatrices(false);
		}
		else
		{
			ComponentTransform2D* transform2D = child->GetComponent<ComponentTransform2D>();
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

	for (std::vector<std::unique_ptr<GameObject>>::iterator it = children.begin(); it != children.end(); ++it)
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

void GameObject::CopyComponent(Component* component)
{
	std::unique_ptr<Component> newComponent;

	ComponentType type = component->GetType();
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

		case ComponentType::PLAYER:
		{
			newComponent = std::make_unique<ComponentPlayer>(static_cast<ComponentPlayer&>(*component));
			break;
		}

		case ComponentType::RIGIDBODY:
		{
			newComponent = std::make_unique<ComponentRigidBody>(static_cast<ComponentRigidBody&>(*component));
			break;
		}

		case ComponentType::SCRIPT:
		{
			newComponent = std::make_unique<ComponentScript>(static_cast<ComponentScript&>(*component));
			break;
		}

		case ComponentType::AUDIOLISTENER:
		{
			newComponent = std::make_unique<ComponentAudioListener>(*static_cast<ComponentAudioListener*>(component));
			break;
		}

		case ComponentType::AUDIOSOURCE:
		{
			newComponent = std::make_unique<ComponentAudioSource>(*static_cast<ComponentAudioSource*>(component));
			break;
		}

		case ComponentType::IMAGE:
		{
			newComponent = std::make_unique<ComponentImage>(*static_cast<ComponentImage*>(component));
			break;
		}

		case ComponentType::BUTTON:
		{
			newComponent = std::make_unique<ComponentButton>(*static_cast<ComponentButton*>(component));
			break;
		}

		case ComponentType::TRANSFORM2D:
		{
			newComponent = std::make_unique<ComponentTransform2D>(*static_cast<ComponentTransform2D*>(component));
			break;
		}

		case ComponentType::CANVAS:
		{
			newComponent = std::make_unique<ComponentCanvas>(*static_cast<ComponentCanvas*>(component));
			break;
		}

		case ComponentType::BREAKABLE:
		{
			newComponent = std::make_unique<ComponentBreakable>(*static_cast<ComponentBreakable*>(component));
			break;
		}

		case ComponentType::ANIMATION:
		{
			newComponent = std::make_unique<ComponentAnimation>(*static_cast<ComponentAnimation*>(component));
			break;
		}

		default:
			LOG_WARNING("Component of type {} could not be copied!", GetNameByType(type).c_str());
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
		case LightType::AREA:
			newComponent = std::make_unique<ComponentAreaLight>(static_cast<ComponentAreaLight&>(*component));
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

	this->Activate();
}

void GameObject::Disable()
{
	assert(parent != nullptr);

	enabled = false;

	this->Deactivate();
}

void GameObject::Activate()
{
	active = parent->IsActive();

	if (!active)
	{
		return;
	}

	for (std::unique_ptr<GameObject>& child : children)
	{
		child->Activate();
	}

	for (std::unique_ptr<Component>& component : components)
	{
		// If the Component is currently disabled itself, avoid sending the signal
		if (component->IsEnabled())
		{
			component->SignalEnable();
		}
	}
}

void GameObject::Deactivate()
{
	active = false;

	for (std::unique_ptr<GameObject>& child : children)
	{
		child->Deactivate();
	}

	for (std::unique_ptr<Component>& component : components)
	{
		// No need to check, we know component->IsEnabled will return false
		component->SignalDisable();
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
		case ComponentType::BREAKABLE:
		{
			newComponent = std::make_unique<ComponentBreakable>(true, this);
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

		case ComponentType::AUDIOSOURCE:
		{
			newComponent = std::make_unique<ComponentAudioSource>(true, this);
			break;
		}

		case ComponentType::AUDIOLISTENER:
		{
			newComponent = std::make_unique<ComponentAudioListener>(true, this);
			break;
		}

		case ComponentType::MESHCOLLIDER:
		{
			newComponent = std::make_unique<ComponentMeshCollider>(true, this);
			break;
		}

		case ComponentType::SCRIPT:
		{
			newComponent = std::make_unique<ComponentScript>(true, this);
			break;
		}
		case ComponentType::CUBEMAP:
		{
			newComponent = std::make_unique<ComponentCubemap>(true, this);
			break;
		}

		default:
			assert(false && "Wrong component type introduced");
			return nullptr;
	}

	if (newComponent)
	{
		Component* referenceBeforeMove = newComponent.get();

		Updatable* updatable = dynamic_cast<Updatable*>(referenceBeforeMove);
		if (updatable)
		{
			App->GetModule<ModuleScene>()->GetLoadedScene()->AddUpdatableObject(updatable);
		}

		components.push_back(std::move(newComponent));
		return referenceBeforeMove;
	}

	return nullptr;
}

Component* GameObject::CreateComponentLight(LightType lightType, AreaType areaType)
{
	std::unique_ptr<Component> newComponent;

	switch (lightType)
	{
		case LightType::DIRECTIONAL:
			newComponent = std::make_unique<ComponentDirLight>(float3(1.0f), 1.0f, this);
			break;

		case LightType::POINT:
			newComponent = std::make_unique<ComponentPointLight>(0.5f, float3(1.0f), 1.0f, this);
			break;

		case LightType::SPOT:
			newComponent = std::make_unique<ComponentSpotLight>(5.0f, 0.15f, 0.3f, float3(1.0f), 1.0f, this);
			break;
		case LightType::AREA:
			newComponent = std::make_unique<ComponentAreaLight>(areaType, this);
			break;
	}

	if (newComponent)
	{
		Component* referenceBeforeMove = newComponent.get();
		components.push_back(std::move(newComponent));
		Scene* scene = App->GetModule<ModuleScene>()->GetLoadedScene();

		switch (lightType)
		{
			case LightType::POINT:
				scene->UpdateScenePointLights();
				scene->RenderPointLights();
				break;

			case LightType::SPOT:
				scene->UpdateSceneSpotLights();
				scene->RenderSpotLights();
				break;
			case LightType::AREA:
				scene->UpdateSceneAreaLights();
				scene->RenderAreaLights();
				break;
		}

		return referenceBeforeMove;
	}

	return nullptr;
}

bool GameObject::RemoveComponent(const Component* component)
{
	auto removeIfResult = std::remove_if(std::begin(components),
										 std::end(components),
										 [&component](const std::unique_ptr<Component>& comp)
										 {
											 return comp.get() == component;
										 });
	if (removeIfResult == std::end(components))
	{
		return false;
	}
	components.erase(removeIfResult, std::end(components));
	return true;
}

GameObject* GameObject::FindGameObject(const std::string& name)
{
	if (this->name == name)
	{
		return this;
	}
	else
	{
		for (std::unique_ptr<GameObject>& child : children)
		{
			GameObject* returnedGO = child->FindGameObject(name);

			if (returnedGO)
			{
				return returnedGO;
			}
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

void GameObject::MoveChild(const GameObject* child, HierarchyDirection direction)
{
	auto childrenVectorBegin = std::begin(children);
	auto childrenVectorEnd = std::end(children);

	auto childIterator = std::find_if(childrenVectorBegin,
									  childrenVectorEnd,
									  [child](const std::unique_ptr<GameObject>& otherChild)
									  {
										  return otherChild.get() == child;
									  });
	if (childIterator == childrenVectorEnd)
	{
		LOG_WARNING("Object being moved ({}) is not a child of this ({})", child, this);
		return;
	}

	auto childToSwap = childrenVectorEnd;
	if (direction == HierarchyDirection::UP)
	{
		if (childIterator == childrenVectorBegin)
		{
			LOG_VERBOSE("Trying to move child ({}) out of children vector bounds", child);
			return;
		}
		childToSwap = std::prev(childIterator);
	}
	else
	{
		if (childIterator == std::prev(childrenVectorEnd))
		{
			LOG_VERBOSE("Trying to move child ({}) out of children vector bounds", child);
			return;
		}
		childToSwap = std::next(childIterator);
	}

	std::iter_swap(childIterator, childToSwap);
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

std::list<GameObject*> GameObject::GetAllDescendants()
{
	std::list<GameObject*> familyObjects = {};
	familyObjects.push_back(this);
	for (std::unique_ptr<GameObject>& children : children)
	{
		std::list<GameObject*> objectsChildren = children->GetAllDescendants();
		familyObjects.insert(familyObjects.end(), objectsChildren.begin(), objectsChildren.end());
	}
	return familyObjects;
}

void GameObject::MoveUpChild(const GameObject* childToMove)
{
	MoveChild(childToMove, HierarchyDirection::UP);
}

void GameObject::MoveDownChild(const GameObject* childToMove)
{
	MoveChild(childToMove, HierarchyDirection::DOWN);
}

void GameObject::SpreadStatic()
{
	for (const std::unique_ptr<GameObject>& child : children)
	{
		child->SetStatic(staticObject);
		child->SpreadStatic();
	}
}

void GameObject::SetStatic(bool newStatic)
{
	staticObject = newStatic;
	std::vector<ComponentRigidBody*> rigids = GetComponents<ComponentRigidBody>();

	for (ComponentRigidBody* rigid : rigids)
	{
		rigid->SetUpMobility();
	}
}