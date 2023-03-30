#pragma once

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

	void UpdateMaterial();
	bool CleanUp();

private:

	void FillBuffers();
	void FillMaterial();
	void FillEBO();

	void CreateInstanceResourceMesh(ResourceMesh* mesh);
	void CreateInstanceResourceMaterial(ResourceMaterial* material);

	ResourceInfo* FindResourceInfo(ResourceMesh* mesh);

	std::vector<ComponentMeshRenderer*> componentsInBatch;
	std::vector<ResourceInfo*> resourcesInfo;
	std::vector<ResourceMaterial*> resourcesMaterial;
	std::vector<int> instanceData;

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
