#include "GameObject.h"
#include "../Components/Component.h"
#include "../Components/ComponentTransform.h"
#include "../Components/ComponentMesh.h"
#include "../Components/ComponentMaterial.h"

#include <assert.h>

GameObject::GameObject(const char* name) : name(name)
{
	uid = UniqueID::GenerateUID();
	CreateComponent(ComponentType::TRANSFORM);
}

GameObject::GameObject(const char* name, GameObject* parent) : name(name), parent(parent)
{
	assert(this->parent != nullptr);

	this->parent->children.push_back(this);
	this->active = (this->parent->IsEnabled() && this->parent->IsActive());

	uid = UniqueID::GenerateUID();
	CreateComponent(ComponentType::TRANSFORM);
}

GameObject::~GameObject()
{
	std::vector<Component*>().swap(components);	// temp vector to properlly deallocate memory
	std::vector<GameObject*>().swap(children);	// temp vector to properlly deallocate memory
}

void GameObject::Update()
{
	for (Component* component : components)
		component->Update();
}

void GameObject::SetParent(GameObject* newParent)
{
	assert(newParent != nullptr);

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

	for (std::vector<GameObject*>::const_iterator it = children.begin(); it != children.end(); ++it)
	{
		if (*it == child)
		{
			children.erase(it);
			return;
		}
	}
}

Component* GameObject::CreateComponent(ComponentType type)
{
	Component* newComponent = nullptr;

	switch (type)
	{
		case ComponentType::TRANSFORM:
		{
			newComponent = new ComponentTransform(true, this);
			break;
		}

		case ComponentType::MESH:
		{
			newComponent = new ComponentTransform(true, this);
			break;
		}

		case ComponentType::MATERIAL:
		{
			newComponent = new ComponentMaterial(true, this);
			break;
		}

		default:
			assert(false && "Unknown Component Type");
	}

	if (newComponent != nullptr)
		components.push_back(newComponent);

	return newComponent;
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