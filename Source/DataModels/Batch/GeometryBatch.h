#pragma once

#include "Math/float3.h"
#include "Math/float2.h"

#include <vector>
#include <map>
#include <unordered_map>

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

struct MaterialMetallic {
	float4 diffuse_color = float4::zero; //0 //16
	int has_diffuse_map = 0;			 //20 //4
	int has_normal_map = 0;				 //24 //4
	int has_metallic_map = 0;			 //36 //4
	float smoothness = 0;				 //28 //4
	float metalness = 0;				 //32 //4
	float normal_strength = 0;			 //16 //4
	uint64_t diffuse_map;				 //40 //8
	uint64_t normal_map;				 //48 //8
	uint64_t metallic_map;				 //56 //8 -->64
};

struct MaterialSpecular {
	float4 diffuse_color = float4::zero;  //0  //16
	float3 specular_color = float3::zero; //16 //16       
	int has_diffuse_map = 0;              //32 //4
	int has_normal_map = 0;               //36 //4
	int has_specular_map = 0;             //40 //4
	float smoothness = 0;                 //44 //4
	float normal_strength = 0;            //48 //4
	uint64_t diffuse_map;				  //48 //8
	uint64_t normal_map;				  //56 //8
	uint64_t specular_map;				  //64 //8    
	float2 padding = float2::zero;		  //72 //8 --> 80
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
	GeometryBatch(int flag);
	~GeometryBatch();

	void CreateVAO();
	void ClearBuffer();
	bool CleanUp();

	void UpdateBatchComponents();

	void AddComponentMeshRenderer(ComponentMeshRenderer* newComponent);

	void DeleteComponent(const ComponentMeshRenderer* componentToDelete);
	void DeleteMaterial(const ComponentMeshRenderer* componentToDelete);
	std::vector< ComponentMeshRenderer*> ChangeBatch(const ComponentMeshRenderer* componentToDelete);

	void BindBatch(bool selected);
	void FillMaterial();
	void ReserveModelSpace();

	bool IsEmpty() const;
	bool IsFillMaterials() const;

	void SetFillMaterials(const bool fillMaterials);

	const int GetFlags() const;

	bool dirtyBatch;

private:
	void FillBuffers();
	void FillEBO();

	void CreateInstanceResourceMesh(ResourceMesh* mesh);
	int CreateInstanceResourceMaterial(const std::shared_ptr<ResourceMaterial> material);

	ResourceInfo* FindResourceInfo(const ResourceMesh* mesh);

	void LockBuffer();
	void WaitBuffer();

	std::vector<ComponentMeshRenderer*> componentsInBatch;
	std::vector<ResourceInfo*> resourcesInfo;
	std::vector<std::shared_ptr<ResourceMaterial>> resourcesMaterial;
	std::vector<int> instanceData;
	std::vector<ComponentMeshRenderer*> componentToMove;
	std::unordered_map<const ComponentMeshRenderer*, int>  objectIndexes;

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
	MaterialMetallic* metallicMaterialData;
	MaterialSpecular* specularMaterialData;
	unsigned int frame = 0;

	std::shared_ptr<ResourceMaterial> defaultMaterial;

	Program* program;

	int flags;
	bool fillMaterials;
};

inline void GeometryBatch::SetFillMaterials(const bool fillMaterials)
{
	this->fillMaterials = fillMaterials;
}

inline const int GeometryBatch::GetFlags() const
{
	return flags;
}

inline void GeometryBatch::ReserveModelSpace()
{
	reserveModelSpace = true;
}

inline bool GeometryBatch::IsEmpty() const
{
	return componentsInBatch.empty();
}

inline bool GeometryBatch::IsFillMaterials() const
{
	return fillMaterials;
}
