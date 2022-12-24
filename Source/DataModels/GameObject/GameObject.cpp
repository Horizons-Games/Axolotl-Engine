#include "GameObject.h"
#include "Components/Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"

#include <assert.h>

GameObject::GameObject(const char* name, GameObject* parent)
{
	this->name = name;

	if (parent != nullptr)
	{
		this->parent = parent;
		this->parent->children.push_back(this);
	}

	Component* transform = new ComponentTransform(true, this);
	components.push_back(transform);

	active = true;
}

GameObject::~GameObject()
{
	components.clear();
	children.clear();
}

void GameObject::Update()
{
	if (active)
	{
		for (int i = 0; i < components.size(); i++)
		{
			components[i]->Update();
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
		}

		case ComponentType::MESH:
		{
			newComponent = new ComponentTransform(true, this);
		}

		case ComponentType::MATERIAL:
		{
			newComponent = new ComponentMaterial(true, this);
			
		}

		default:
			assert(false && "Unknown Component Type");
	}

	if (newComponent != nullptr)
		components.push_back(newComponent);

	return newComponent;
}