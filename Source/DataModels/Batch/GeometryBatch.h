#pragma once

#include "FileSystem/UniqueID.h"

#include <vector>
#include "GL/glew.h"

class ComponentMeshRenderer;
class ResourceMesh;
class GameObject;

struct Command
{
	GLuint  count; // Number of indices in the mesh
	GLuint  instanceCount = 1; // Number of instances to render
	GLuint  firstIndex = 0; // Index offset in the EBO
	GLuint  baseVertex; // Vertex offset in the VBO
	GLuint  baseInstance; // Instance Index
};

class GeometryBatch
{
public:
	GeometryBatch();
	~GeometryBatch();

	void AddComponentMeshRenderer(ComponentMeshRenderer* newComponent);

	void BindBatch();

	const int GetFlags() const;
	const int GetResourceIndex() const;

	bool CleanUp();

private:
	void AddUniqueComponent(ResourceMesh* resourceMesh);
	const GameObject* GetComponentOwner(const ResourceMesh* resourceMesh);
	bool isUniqueResourceMesh(const ResourceMesh* resourceMesh);

	std::vector<ComponentMeshRenderer*> components;
	std::vector<ResourceMesh*> resourceMeshes;

	unsigned int vbo = 0;
	unsigned int ebo = 0;
	unsigned int vao = 0;
	unsigned int indirectBuffer = 0;
	unsigned int resourceMesheIndex;
	std::vector<Command> command;
	//Command command[100];

	int flags = 0;;
};

inline const int GeometryBatch::GetFlags() const
{
	return flags;
}

inline const int GeometryBatch::GetResourceIndex() const
{
	return resourceMesheIndex;
}
