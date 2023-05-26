#pragma once

#include "Math/float3.h"
#include "Math/float2.h"

#include <vector>
#include <unordered_map>
#include <memory>

#include "GL/glew.h"

#define DOUBLE_BUFFERS 2

class ComponentMeshRenderer;
class ResourceMesh;
class ResourceMaterial;
class GameObject;
class Program;

class GeometryBatch
{
public:
	GeometryBatch(int flag);
	~GeometryBatch();

	void CreateVAO();
	void ClearBuffer();
	void CleanUp();

	void UpdateBatchComponents();

	void AddComponentMeshRenderer(ComponentMeshRenderer* newComponent);

	void DeleteComponent(ComponentMeshRenderer* componentToDelete);
	void DeleteMaterial(const ComponentMeshRenderer* componentToDelete);
	std::vector< ComponentMeshRenderer*> ChangeBatch(const ComponentMeshRenderer* componentToDelete);

	void BindBatch(bool selected);
	void FillMaterial();
	void ReserveModelSpace();

	bool IsEmpty() const;
	bool IsDirty() const;

	void SetFillMaterials(const bool fillMaterials);
	void SortByDistanceFarToClose();
	void SortByDistanceCloseToFar();
	void SetDirty(const bool dirty);

	const int GetFlags() const;

private:
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
		std::shared_ptr<ResourceMesh> resourceMesh;
		unsigned int vertexOffset;
		unsigned int indexOffset;
	};

	struct PerInstance
	{
		unsigned int numBones;
		unsigned int paletteOffset;
		unsigned int padding0, padding1;
	};

	struct uint4
	{
		unsigned int x, y, z, w;
	};

private:
	void FillBuffers();
	void FillEBO();

	void CreateInstanceResourceMesh(std::shared_ptr<ResourceMesh> mesh);
	int CreateInstanceResourceMaterial(const std::shared_ptr<ResourceMaterial> material);

	ResourceInfo* FindResourceInfo(const std::shared_ptr<ResourceMesh> mesh);

	void LockBuffer();
	void WaitBuffer();

	std::vector<ComponentMeshRenderer*> componentsInBatch;
	std::vector<std::shared_ptr<ResourceMaterial>> resourcesMaterial;
	std::unordered_map<const ComponentMeshRenderer*, int>  objectIndexes;
	std::unordered_map<const ComponentMeshRenderer*, int>  paletteIndexes;
	std::vector<ResourceInfo*> resourcesInfo;
	std::vector<PerInstance> perInstances;
	std::vector<int> instanceData;

	unsigned int ebo;
	unsigned int vao;
	
	unsigned int verticesBuffer;
	unsigned int textureBuffer;
	unsigned int normalsBuffer;
	unsigned int tangentsBuffer;
	unsigned int bonesBuffer;
	unsigned int weightsBuffer;
	unsigned int indirectBuffer = 0;
	unsigned int materials = 0;
	unsigned int perInstancesBuffer = 0;
	unsigned int transforms[DOUBLE_BUFFERS] = { 0, 0 };
	unsigned int palettes[DOUBLE_BUFFERS] = { 0, 0 };

	bool createBuffers;
	bool reserveModelSpace;
	bool dirtyBatch;

	unsigned int numTotalVertices;
	unsigned int numTotalIndices;
	unsigned int numTotalFaces;
	unsigned int frame;

	GLsync gSync[DOUBLE_BUFFERS] = { nullptr, nullptr };

	const GLuint bindingPointPalette = 7;
	const GLuint bindingPointPerInstance = 6;
	const GLuint bindingPointModel = 10;
	const GLuint bindingPointMaterial = 11;

	GLbitfield mapFlags;
	GLbitfield createFlags;

	float4x4* transformData[DOUBLE_BUFFERS];
	float4x4* paletteData[DOUBLE_BUFFERS];
	MaterialMetallic* metallicMaterialData;
	MaterialSpecular* specularMaterialData;

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

inline bool GeometryBatch::IsDirty() const
{
	return dirtyBatch;
}