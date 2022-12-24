#include "GameObject.h"
#include "Components/Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"

#include <assert.h>

GameObject::GameObject(const char* name, GameObject* parent)
{
	this->name = name;
	this->parent = parent;

	if (this->parent != nullptr)
		this->parent->children.push_back(this);

	Component* transform = CreateComponent(ComponentType::TRANSFORM);
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
	switch (type)
	{
		case ComponentType::TRANSFORM:
		{
			ComponentTransform newComponent = ComponentTransform(true, this);
			return &newComponent;
		}

		case ComponentType::MESH:
		{
			ComponentTransform newComponent = ComponentTransform(true, this);
			return &newComponent;
		}

		case ComponentType::MATERIAL:
		{
			ComponentMaterial newComponent = ComponentMaterial(true, this);
			return &newComponent;
		}

		default:
			assert(false && "Unknown Component Type");
	}
}