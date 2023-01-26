#pragma once

#include "FileSystem/UniqueID.h"

#include <memory>
#include <vector>

class ComponentMeshRenderer;
class ResourceMesh;
class GameObject;

class GeometryBatch
{
public:
	GeometryBatch();
	~GeometryBatch();

	void AddComponentMeshRenderer(const std::shared_ptr<ComponentMeshRenderer>& newComponent);

	void Draw();

	const std::weak_ptr<ResourceMesh>& GetMesh(const UID meshUID) const;

private:
	void AddUniqueComponent(const std::weak_ptr<ResourceMesh>& resourceMesh);
	const std::shared_ptr<GameObject>& GetComponentOwner(const std::weak_ptr<ResourceMesh>& resourceMesh);
	bool isUniqueResourceMesh(const std::weak_ptr<ResourceMesh>& resourceMesh);

	std::vector<std::shared_ptr<ComponentMeshRenderer>> components;
	std::vector<std::weak_ptr<ResourceMesh>> uniqueComponents;

	unsigned int vbo = 0;
	unsigned int ebo = 0;
	unsigned int vao = 0;
};
