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
	components.push_back(newComponent);
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

void GeometryBatch::CreateUniqueComponent()
{

}