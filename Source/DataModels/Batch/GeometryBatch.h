#pragma once

#include "Math/float3.h"

#include <vector>

#include "GL/glew.h"

#define DOUBLE_BUFFERS 2

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
	float4 diffuse_color = float4::zero;	//0 //16
	float normal_strength = 0;				//16 //4
	int has_diffuse_map = 0;				//20 //4
	int has_normal_map = 0;					//24 //4
	float smoothness = 0;					//28 //4
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

	void CreateVAO();
	void ClearBuffer();
	bool CleanUp();

	void UpdateBatchComponents();

	void AddComponentMeshRenderer(ComponentMeshRenderer* newComponent);

	void DeleteComponent(ComponentMeshRenderer* componentToDelete);
	void DeleteMaterial(ComponentMeshRenderer* componentToDelete);

	void BindBatch();
	void FillMaterial();
	void ReserveModelSpace();

	const int GetFlags() const;

	bool dirtyBatch;

private:
	void FillBuffers();
	void FillEBO();

	void CreateInstanceResourceMesh(ResourceMesh* mesh);
	void CreateInstanceResourceMaterial(ResourceMaterial* material);

	ResourceInfo* FindResourceInfo(ResourceMesh* mesh);

	void LockBuffer();
	void WaitBuffer();

	std::vector<ComponentMeshRenderer*> componentsInBatch;
	std::vector<ResourceInfo*> resourcesInfo;
	std::vector<ResourceMaterial*> resourcesMaterial;
	std::vector<int> instanceData;

	unsigned int ebo;
	unsigned int vao;
	
	unsigned int indirectBuffer;
	unsigned int verticesBuffer;
	unsigned int textureBuffer;
	unsigned int normalsBuffer;
	unsigned int tangentsBuffer;
	unsigned int transforms[DOUBLE_BUFFERS];
	unsigned int materials;

	bool createBuffers;
	bool reserveModelSpace;

	unsigned int numTotalVertices;
	unsigned int numTotalIndices;
	unsigned int numTotalFaces;


	GLsync gSync[DOUBLE_BUFFERS] = { nullptr,nullptr };

	const GLuint bindingPointModel = 10;
	const GLuint bindingPointMaterial = 11;

	GLbitfield mapFlags;
	GLbitfield createFlags;
	float4x4* transformData[DOUBLE_BUFFERS];
	Material* materialData;
	unsigned int frame = 0;

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
