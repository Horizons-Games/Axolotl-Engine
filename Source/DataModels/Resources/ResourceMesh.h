#pragma once
#pragma warning (disable: 26495)

#include "Resource.h"

#include "Math/float3.h"

struct OptionsMesh
{
};

struct BindBone
{
	float4x4 transform;
	std::string name;
};

struct BindAttach
{
	unsigned bones[4] = {0u, 0u, 0u, 0u};
	float weights[4] = {0.0f, 0.0f, 0.0f, 0.0f};
};

class ResourceMesh : virtual public Resource
{
public:
	ResourceMesh(UID resourceUID, 
		const std::string& fileName, 
		const std::string& assetsPath, 
		const std::string& libraryPath);
	virtual ~ResourceMesh() override;

	ResourceType GetType() const override;

	void SaveImporterOptions(Json& meta) override {};
	void LoadImporterOptions(Json& meta) override {};

	unsigned int GetVBO() const;
	unsigned int GetEBO() const;
	unsigned int GetVAO() const;

	unsigned int GetNumVertices();
	unsigned int GetNumFaces();
	unsigned int GetNumIndexes();
	unsigned int GetMaterialIndex();
	const std::vector<float3>& GetVertices();
	const std::vector<float3>& GetTextureCoords();
	const std::vector<float3>& GetNormals();
	const std::vector<float3>& GetTangents();
	const std::vector<std::vector<unsigned int> >& GetFacesIndices();

	OptionsMesh& GetOptions();

	void SetNumVertices(unsigned int numVertices);
	void SetNumFaces(unsigned int numFaces);
	void SetNumIndexes(unsigned int numIndexes);
	void SetMaterialIndex(unsigned int materialIndex);
	void SetVertices(const std::vector<float3>& vertices);
	void SetTextureCoords(const std::vector<float3>& textureCoords);
	void SetNormals(const std::vector<float3>& normals);
	void SetTangents(const std::vector<float3>& tangents);
	void SetFacesIndices(const std::vector<std::vector<unsigned int> >& facesIndices);

	const std::vector<Triangle> RetrieveTriangles(const float4x4& modelMatrix);

protected:
	void InternalLoad() override;
	void InternalUnload() override;

private:
	void CreateVBO();
	void CreateEBO();
	void CreateVAO();

	unsigned int vbo;
	unsigned int ebo;
	unsigned int vao;

	//parameters for buffer object creation
	unsigned int numVertices;
	unsigned int numFaces;
	unsigned int numIndexes;
	unsigned int materialIndex;
	std::vector<float3> vertices;
	std::vector<float3> textureCoords;
	std::vector<float3> normals;
	std::vector<float3> tangents{};
	std::vector<std::vector<unsigned int> > facesIndices;
	std::vector<BindBone> bones;
	std::vector<BindAttach> attaches;

	OptionsMesh options;
};

inline ResourceType ResourceMesh::GetType() const
{
	return ResourceType::Mesh;
}

inline unsigned int ResourceMesh::GetVBO() const
{
	return vbo;
}

inline unsigned int ResourceMesh::GetEBO() const
{
	return ebo;
}

inline unsigned int ResourceMesh::GetVAO() const
{
	return vao;
}

inline unsigned int ResourceMesh::GetNumVertices()
{
	return numVertices;
}

inline unsigned int ResourceMesh::GetNumFaces()
{
	return numFaces;
}

inline unsigned int ResourceMesh::GetNumIndexes()
{
	return numIndexes;
}

inline unsigned int ResourceMesh::GetMaterialIndex()
{
	return materialIndex;
}

inline const std::vector<float3>& ResourceMesh::GetVertices()
{
	return vertices;
}

inline const std::vector<float3>& ResourceMesh::GetTextureCoords()
{
	return textureCoords;
}

inline const std::vector<float3>& ResourceMesh::GetNormals()
{
	return normals;
}

inline const std::vector<float3>& ResourceMesh::GetTangents()
{
	return tangents;
}

inline const std::vector<std::vector<unsigned int> >& ResourceMesh::GetFacesIndices()
{
	return facesIndices;
}

inline OptionsMesh& ResourceMesh::GetOptions()
{
	return options;
}

inline void ResourceMesh::SetNumVertices(unsigned int numVertices)
{
	this->numVertices = numVertices;
}

inline void ResourceMesh::SetNumFaces(unsigned int numFaces)
{
	this->numFaces = numFaces;
	numIndexes = numFaces * 3;
}

inline void ResourceMesh::SetNumIndexes(unsigned int numIndexes)
{
	this->numIndexes = numIndexes;
}

inline void ResourceMesh::SetMaterialIndex(unsigned int materialIndex)
{
	this->materialIndex = materialIndex;
}

inline void ResourceMesh::SetVertices(const std::vector<float3>& vertices)
{
	this->vertices = vertices;
}

inline void ResourceMesh::SetTextureCoords(const std::vector<float3>& textureCoords)
{
	this->textureCoords = textureCoords;
}

inline void ResourceMesh::SetNormals(const std::vector<float3>& normals)
{
	this->normals = normals;
}

inline void ResourceMesh::SetTangents(const std::vector<float3>& tangents)
{
	this->tangents = tangents;
}

inline void ResourceMesh::SetFacesIndices(const std::vector<std::vector<unsigned int> >& facesIndices)
{
	this->facesIndices = facesIndices;
}
