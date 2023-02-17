#pragma once

#include "FileSystem/UniqueID.h"

#include <vector>

class ComponentMeshRenderer;
class ResourceMesh;
class GameObject;

class GeometryBatch
{
public:
	GeometryBatch();
	~GeometryBatch();

	void AddComponentMeshRenderer(ComponentMeshRenderer* newComponent);

	void Draw();

	const int GetFlags() const;

private:
	void AddUniqueComponent(ResourceMesh* resourceMesh);
	const GameObject* GetComponentOwner(const ResourceMesh* resourceMesh);
	bool isUniqueResourceMesh(const ResourceMesh* resourceMesh);

	std::vector<ComponentMeshRenderer*> components;
	std::vector<ResourceMesh*> uniqueComponents;

	unsigned int vbo = 0;
	unsigned int ebo = 0;
	unsigned int vao = 0;

	int flags = 0;;
};

inline const int GeometryBatch::GetFlags() const
{
	return flags;
}
