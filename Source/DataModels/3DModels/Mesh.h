#pragma once

#include <vector>

#include "assimp/mesh.h"
#include "Math/float3.h"
#include "Math/float4x4.h"

class Mesh
{
public:
	Mesh(const aiMesh* mesh);
	~Mesh();

	void LoadVBO(const aiMesh* mesh);
	void LoadEBO(const aiMesh* mesh);
	void CreateVAO();
	void Draw(const std::vector<unsigned>& modelTextures, 
		const float3 &translation, const float4x4 &rotation, const float3 &scale);

	vec* GetVertices() const;
	unsigned GetNumVertices() const;
	unsigned GetNumTriangles() const;
	
	unsigned numVertices = 0;
	unsigned numIndexes  = 0;

private:
	unsigned vbo;
	unsigned ebo;
	unsigned vao;
	
	unsigned materialIndex;

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
