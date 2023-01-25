#pragma once

#include "FileSystem/UniqueID.h"

#include <memory>
#include <vector>

class ComponentMeshRenderer;
class ResourceMesh;

class GeometryBatch
{
public:
	GeometryBatch();
	~GeometryBatch();

	void AddComponentMeshRenderer(ComponentMeshRenderer* newComponent);

	std::weak_ptr<ResourceMesh> GetMesh(UID meshUID) const;

private:
	void CreateUniqueComponent();

	std::vector<ComponentMeshRenderer*> components;
	std::vector<std::weak_ptr<ResourceMesh>> uniqueComponents;

	unsigned int vbo = 0;
	unsigned int ebo = 0;
	unsigned int vao = 0;
};
