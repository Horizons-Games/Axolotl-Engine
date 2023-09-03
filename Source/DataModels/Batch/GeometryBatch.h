#pragma once

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
	std::vector<ComponentMeshRenderer*> ChangeBatch(const ComponentMeshRenderer* componentToDelete);

	void BindBatch(bool selected);
	void BindBatch(std::vector<GameObject*>& objects);
	void FillMaterial();
	void ReserveModelSpace();

	bool IsEmpty() const;
	bool IsDirty() const;

	void SetFillMaterials(const bool fillMaterials);
	void SortByDistanceFarToClose();
	void SortByDistanceCloseToFar();
	void SortByDistanceFarToClose(const float3& position);
	void SortByDistanceCloseToFar(const float3& position);
	void SetDirty(const bool dirty);

	const int GetFlags() const;

private:
	struct Command
	{
		GLuint count;		  // Number of indices in the mesh
		GLuint instanceCount; // Number of instances to render
		GLuint firstIndex;	  // Index offset in the EBO
		GLuint baseVertex;	  // Vertex offset in the VBO
		GLuint baseInstance;  // Instance Index
	};

	struct MaterialMetallic 
	{
		float4 diffuse_color = float4::zero;	//0 //16
		int has_diffuse_map = 0;				//16 //4
		int has_normal_map = 0;					//20 //4
		int has_metallic_map = 0;				//24 //4
		int has_emissive_map = 0;				//28 //4
		float smoothness = 0;					//32 //4
		float metalness = 0;					//36 //4
		float normal_strength = 0;				//40 //4
		float intensityBloom = 0;				//44 //4
		uint64_t diffuse_map;					//48 //8
		uint64_t normal_map;					//56 //8
		uint64_t metallic_map;					//64 //8
		uint64_t emissive_map;					//72 //8 --> 80
	};

	struct MaterialSpecular 
	{
		float4 diffuse_color = float4::zero;	//0  //16
		float3 specular_color = float3::zero;	//16 //16
		int has_diffuse_map = 0;				//32 //4
		int has_normal_map = 0;					//36 //4
		int has_specular_map = 0;				//40 //4
		int has_emissive_map = 0;				//44 //4
		float smoothness = 0;					//48 //4
		float normal_strength = 0;				//52 //4
		float intensityBloom = 0;				//56 //4
		int padding;
		uint64_t diffuse_map;					//64 //8
		uint64_t normal_map;					//72 //8
		uint64_t specular_map;					//80 //8
		uint64_t emissive_map;					//88 //8 --> 96
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

	struct Tiling
	{
		float2 tiling;
		float2 offset;
		float2 percentage;
		float2 padding;
	};

	struct Effect
	{
		float3 color;
		int discard; // works like a boolean
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
	std::unordered_map<ComponentMeshRenderer*, int> objectIndexes;
	std::unordered_map<const ComponentMeshRenderer*, int> paletteIndexes;
	std::vector<ResourceInfo*> resourcesInfo;
	std::vector<PerInstance> perInstances;
	std::vector<int> instanceData;

	unsigned int ebo = 0;
	unsigned int vao = 0;

	unsigned int verticesBuffer = 0;
	unsigned int textureBuffer = 0;
	unsigned int normalsBuffer = 0;
	unsigned int tangentsBuffer = 0;
	unsigned int bonesBuffer = 0;
	unsigned int weightsBuffer = 0;
	unsigned int indirectBuffer = 0;
	unsigned int materials = 0;
	unsigned int perInstancesBuffer = 0;
	unsigned int transforms[DOUBLE_BUFFERS] = { 0, 0 };
	unsigned int palettes[DOUBLE_BUFFERS] = { 0, 0 };
	unsigned int tilingBuffer = 0;
	unsigned int effectBuffer = 0;

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
	const GLuint bindingPointTiling = 12;
	const GLuint bindingPointEffect = 13;

	GLbitfield mapFlags;
	GLbitfield createFlags;

	float4x4* transformData[DOUBLE_BUFFERS];
	float4x4* paletteData[DOUBLE_BUFFERS];
	Tiling* tilingData;
	Effect* effectData;
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