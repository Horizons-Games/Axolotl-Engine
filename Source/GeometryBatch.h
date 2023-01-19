#pragma once

#include <memory>

class ComponentMesh;
class ResourceMesh;

class GeometryBatch
{
public:
	GeometryBatch();
	~GeometryBatch();

private:
	std::shared_ptr<ComponentMesh> components;
	std::shared_ptr <ResourceMesh> uniqueComponents;

	unsigned int vbo = 0;
	unsigned int ebo = 0;
	unsigned int vao = 0;
};
