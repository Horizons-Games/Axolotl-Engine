#include "GameObject.h"
#include "Components/Component.h"
#include "../Components/ComponentTransform.h"
#include "../Components/ComponentMesh.h"
#include "../Components/ComponentMaterial.h"

#include <assert.h>

GameObject::GameObject(const char* name) : name(name)
{
	CreateComponent(ComponentType::TRANSFORM);
}

GameObject::GameObject(const char* name, GameObject* parent) : name(name), parent(parent)
{
	assert(this->parent != nullptr);

	this->parent->children.push_back(this);
	this->active = this->parent->GetActive();

	CreateComponent(ComponentType::TRANSFORM);
}

GameObject::~GameObject()
{
	components.clear();
	std::vector<Component*>().swap(components);

	children.clear();
	std::vector<GameObject*>().swap(children);
}

void GameObject::Init()
{
	id = GenerateUID();
}

void GameObject::Update()
{
	for (Component* component : components)
	{
		component->Update();
	}

	for (GameObject* child : children)
	{
		child->Update();
	}

	if (active)
	{
		for (Component* component : components)
			component->Update();
	}
}

void GameObject::SetParent(GameObject* newParent)
{
	assert(newParent != nullptr);

	parent->RemoveChild(this);
	parent = newParent;
	parent->AddChild(this);
}

void GameObject::AddChild(GameObject* child)
{
	assert(child != nullptr);

	if (!IsAChild(child))
		children.push_back(child);
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

void GameObject::DrawGizmos()
{
	for (Component* component : components)
	{
		component->DrawGizmos();
	}

	for (GameObject* child : children)
	{
		child->DrawGizmos();
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

	bool isAChild = false;

	for (GameObject* gameObject : children)
	{
		if (gameObject == child)
			isAChild = true;
	}

	return isAChild;

}