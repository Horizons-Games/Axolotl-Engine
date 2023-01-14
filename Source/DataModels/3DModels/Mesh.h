#pragma once

#include <vector>

#include "assimp/mesh.h"
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "DataModels/Resources/ResourceMesh.h"

struct PointLight
{
	float4 position;
	float4 color;
};

struct SpotLight
{
	float4 position;
	float4 color;
	float3 aim;
	float innerAngle;
	float outAngle;
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	void Load(const aiMesh* mesh);

	void SetFromResource(std::shared_ptr<ResourceMesh>& resource);

	void LoadVBO(const aiMesh* mesh);
	void LoadEBO(const aiMesh* mesh);
	void CreateVAO();
	void Draw(const std::vector<unsigned>& modelTextures, 
		const float3 &translation, const float4x4 &rotation, const float3 &scale);
	
	void GenerateLights();
	void RenderLights();

	vec* GetVertices() const;
	unsigned GetNumVertices() const;
	unsigned GetNumTriangles() const;
	
private:
	unsigned vbo;
	unsigned ebo;
	unsigned vao;
	
	unsigned uboAmbient;
	unsigned uboDirectional;
	unsigned ssboPoint;
	unsigned ssboSpot;

	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;

	unsigned materialIndex;
	unsigned numVertices;
	unsigned numIndexes;

	float4x4 modelTransform = float4x4::identity;

	vec* vertices;
};

inline vec* Mesh::GetVertices() const
{
	return this->vertices;
}

inline unsigned Mesh::GetNumVertices() const
{
	return this->numVertices;
}

inline unsigned Mesh::GetNumTriangles() const
{
	return this->numIndexes / 3;
}
