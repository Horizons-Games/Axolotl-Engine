#include "GeometryBatch.h"

#include "Components/ComponentMeshRenderer.h"

GeometryBatch::GeometryBatch()
{}

GeometryBatch::~GeometryBatch()
{
	components.clear();
	uniqueComponents.clear();
}

void GeometryBatch::AddComponentMeshRenderer(ComponentMeshRenderer* newComponent)
{
	if (newComponent != nullptr)
	{
		AddUniqueComponent(newComponent->GetMesh());
		components.push_back(newComponent);
	}
}

std::weak_ptr<ResourceMesh> GeometryBatch::GetMesh(UID meshUID) const
{
	for (ComponentMeshRenderer* component : components)
	{
		if (component->GetMeshUID() == meshUID)
		{
			return component->GetMesh();
		}
	}

	return std::weak_ptr<ResourceMesh>();
}

void GeometryBatch::AddUniqueComponent(std::weak_ptr<ResourceMesh> resourceMesh)
{
	if (isUniqueResourceMesh(resourceMesh))
	{
		uniqueComponents.push_back(resourceMesh);
	}
}

bool GeometryBatch::isUniqueResourceMesh(std::weak_ptr<ResourceMesh> resourceMesh)
{
	std::shared_ptr<ResourceMesh> meshAsShared = resourceMesh.lock();

	for (std::weak_ptr<ResourceMesh> uniqueComponent : uniqueComponents)
	{
		if (uniqueComponent.lock() == meshAsShared)
		{
			return false;
		}
	}

	return true;
}