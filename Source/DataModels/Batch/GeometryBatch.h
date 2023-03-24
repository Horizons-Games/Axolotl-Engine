#pragma once

#include "FileSystem/UniqueID.h"
#include "Math/float2.h"
#include "Math/float3.h"

#include <vector>

#include "GL/glew.h"

class ComponentMeshRenderer;
class ResourceMesh;
class ResourceMaterial;
class GameObject;

struct Command
{
	GLuint  count;			// Number of indices in the mesh
	GLuint  instanceCount;	// Number of instances to render
	GLuint  firstIndex;		// Index offset in the EBO
	GLuint  baseVertex;		// Vertex offset in the VBO
	GLuint  baseInstance;	// Instance Index
};

struct Material {
	float3 diffuse_color = float3::zero;      
	float normal_strength = 0;
	int has_diffuse_map   =0;   
	int has_normal_map    =0;  
	float smoothness = 0;
	int has_metallic_alpha = 0;
	float metalness = 0;           //32 //4 //location 9
	int has_metallic_map = 0;      //36 //4 //location 10
};

struct ResourceInfo //temporary name
{
	ResourceMesh* resourceMesh;
	ResourceMaterial* resourceMaterial;
	int vertexOffset;
	int indexOffset;
};

class GeometryBatch
{
public:
	GeometryBatch();
	~GeometryBatch();

	void CreateVAO();

	void AddComponentMeshRenderer(ComponentMeshRenderer* newComponent);
	void DeleteComponent(ComponentMeshRenderer* componentToDelete);

	void BindBatch(const std::vector<ComponentMeshRenderer*>& componentsToRender);

	const int GetFlags() const;
	const int GetResourceIndex() const;

	bool CleanUp();

private:
	void FillEBO();

	void FillBuffers();

	const GameObject* GetComponentOwner(const ResourceMesh* resourceMesh); //delete

	void CreateInstance(ResourceMesh* mesh, ResourceMaterial* material);

	ResourceInfo& FindResourceInfo(ResourceMesh* mesh);

	std::vector<ComponentMeshRenderer*> components;
	std::vector<ResourceInfo> resourcesInfo;

	unsigned int ebo = 0;
	unsigned int vao = 0;
	
	unsigned int indirectBuffer = 0;
	unsigned int verticesBuffer = 0;
	unsigned int textureBuffer = 0;
	unsigned int normalsBuffer = 0;
	unsigned int tangentsBuffer = 0;
	unsigned int transforms = 0;
	unsigned int materials = 0;

	bool createBuffers = true;
	bool reserveModelSpace = true;

	unsigned int resourceMeshIndex = 0;

	std::vector<Command> commands;
	//std::vector<float4x4> modelMatrices;
	//std::vector<float4x4> storageModel;

	int numTotalVertices = 0;
	int numTotalIndices = 0;
	int numTotalFaces = 0;

	int flags = 0;
	bool isloaded = false;
};

inline const int GeometryBatch::GetFlags() const
{
	return flags;
}

inline const int GeometryBatch::GetResourceIndex() const
{
	return resourceMeshIndex;
}
