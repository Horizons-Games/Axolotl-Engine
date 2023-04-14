#pragma once

#include "Math/float2.h"
#include "Math/float3.h"
#include "Resources/ResourceMaterial.h"

#include <vector>

#include "GL/glew.h"

class ComponentMeshRenderer;
class ResourceMesh;
class ResourceMaterial;
class GameObject;
class Program;

struct Command
{
	GLuint  count;			// Number of indices in the mesh
	GLuint  instanceCount;	// Number of instances to render
	GLuint  firstIndex;		// Index offset in the EBO
	GLuint  baseVertex;		// Vertex offset in the VBO
	GLuint  baseInstance;	// Instance Index
};

struct Material {
	float3 diffuse_color = float3::zero;	//0 //12
	float normal_strength = 0;				//12 //4
	int has_diffuse_map = 0;				//16 //4
	int has_normal_map = 0;					//20 //4
	float smoothness = 0;					//24 //4
	int has_metallic_alpha = 0;				//28 //4
	float metalness = 0;					//32 //4
	int has_metallic_map = 0;				//36 //4
	uint64_t diffuse_map;					//40 //8
	uint64_t normal_map;					//48 //8
	uint64_t metallic_map;					//56 //8	//-->64
};

struct ResourceInfo
{
	ResourceMesh* resourceMesh;
	unsigned int vertexOffset;
	unsigned int indexOffset;
};

class GeometryBatch
{
public:
	GeometryBatch();
	~GeometryBatch();

	const int GetFlags() const;

	void CreateVAO();

	void AddComponentMeshRenderer(ComponentMeshRenderer* newComponent);
	void DeleteComponent(ComponentMeshRenderer* componentToDelete);
	
	void DeleteMaterial(ComponentMeshRenderer* componentToDelete);

	void BindBatch(const std::vector<ComponentMeshRenderer*>& componentsToRender);

	void FillMaterial();

	void ReserveModelSpace();

	bool CleanUp();

private:

	void FillBuffers();
	void FillEBO();

	void CreateInstanceResourceMesh(ResourceMesh* mesh);
	void CreateInstanceResourceMaterial(ResourceMaterial* material);

	ResourceInfo* FindResourceInfo(ResourceMesh* mesh);

	std::vector<ComponentMeshRenderer*> componentsInBatch;
	std::vector<ResourceInfo*> resourcesInfo;
	std::vector<ResourceMaterial*> resourcesMaterial;
	std::vector<int> instanceData;
	int maxSize;

	unsigned int ebo;
	unsigned int vao;
	
	unsigned int indirectBuffer;
	unsigned int verticesBuffer;
	unsigned int textureBuffer;
	unsigned int normalsBuffer;
	unsigned int tangentsBuffer;
	unsigned int transforms;
	unsigned int materials;

	bool createBuffers;
	bool reserveModelSpace;

	unsigned int numTotalVertices;
	unsigned int numTotalIndices;
	unsigned int numTotalFaces;

	ResourceMaterial* defaultMaterial;

	Program* program;

	int flags;
};

inline const int GeometryBatch::GetFlags() const
{
	return flags;
}

inline void GeometryBatch::ReserveModelSpace()
{
	reserveModelSpace = true;
}
