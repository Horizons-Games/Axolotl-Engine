#include "GameObject.h"
#include "Components/Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"

#include "EngineLog.h"

GameObject::GameObject()
{
	Component* transform = CreateComponent(ComponentType::TRANSFORM);
	components.push_back(transform);
}

GameObject::~GameObject()
{
}

void GameObject::Update()
{
	for (int i = 0; i < components.size(); i++)
	{
		components[i]->Update();
	}
}

Component* GameObject::CreateComponent(ComponentType type)
{
	if (type == ComponentType::TRANSFORM)
	{
		ComponentTransform newComponent = ComponentTransform(true, this);
		return &newComponent;
	}

	if (type == ComponentType::MESH)
	{
		ComponentMesh newComponent = ComponentMesh(true, this);
		return &newComponent;
	}

	if (type == ComponentType::MATERIAL)
	{
		ComponentMaterial newComponent = ComponentMaterial(true, this);
		return &newComponent;
	}
}