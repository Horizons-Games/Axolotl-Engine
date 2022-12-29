#include "GameObject.h"
#include "Components/Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"

#include <assert.h>

GameObject::GameObject(const char* name) : name(name)
{
	Component* transform = new ComponentTransform(true, this);
	components.push_back(transform);
}

GameObject::GameObject(const char* name, GameObject* parent) : name(name), parent(parent)
{
	if(this->parent != nullptr)
		this->parent->children.push_back(this);

	Component* transform = new ComponentTransform(true, this);
	components.push_back(transform);
}

GameObject::~GameObject()
{
	components.clear();
	std::vector<Component*>().swap(components);

	children.clear();
	std::vector<GameObject*>().swap(children);
}

void GameObject::Update()
{
	if (active)
	{
		for (Component* component : components)
			component->Update();
	}
}

void GameObject::SetParent(GameObject* parent)
{
	if (this->parent != nullptr)
	{
		this->parent->RemoveChild(this);
		this->parent = parent;
		this->parent->AddChild(this);
	}	
}

void GameObject::AddChild(GameObject* child)
{
	if (!IsAChild(child))
		children.push_back(child);
}

void GameObject::RemoveChild(GameObject* child)
{
	if (IsAChild(child))
	{
		for (std::vector<GameObject*>::const_iterator it = children.begin(); it != children.end(); ++it)
		{
			if (*it == child)
			{
				children.erase(it);
				return;
			}
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
	bool isAChild = false;

	for (GameObject* gameObject : children)
	{
		if (gameObject == child)
			isAChild = true;
	}

	return isAChild;
}