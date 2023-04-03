#pragma once
#pragma warning (disable: 26495)

#include "Resource.h"

#include "Math/float3.h"
#include "Math/float4x4.h"

struct Bone
{
	float4x4 transform;
	std::string name;
};

struct Attach
{
	unsigned int bones[4] = {0u, 0u, 0u, 0u};
	float weights[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	unsigned int numBones = 0u;
};

struct OptionsMesh
{
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
	unsigned int GetNumBones();
	unsigned int GetMaterialIndex();
	const std::vector<float3>& GetVertices();
	const std::vector<float3>& GetTextureCoords();
	const std::vector<float3>& GetNormals();
	const std::vector<float3>& GetTangents();
	const std::vector<std::vector<unsigned int> >& GetFacesIndices();
	const std::vector<Bone>& GetBones() const ;
	const std::vector<Attach>& GetAttaches() const;
	const std::vector<unsigned int>& GetNumWeights() const;

	OptionsMesh& GetOptions();

	void SetNumVertices(unsigned int numVertices);
	void SetNumFaces(unsigned int numFaces);
	void SetNumIndexes(unsigned int numIndexes);
	void SetNumBones(unsigned int numBones);
	void SetMaterialIndex(unsigned int materialIndex);
	void SetVertices(const std::vector<float3>& vertices);
	void SetTextureCoords(const std::vector<float3>& textureCoords);
	void SetNormals(const std::vector<float3>& normals);
	void SetTangents(const std::vector<float3>& tangents);
	void SetFacesIndices(const std::vector<std::vector<unsigned int> >& facesIndices);
	void SetBones(const std::vector<Bone>& bones);
	void SetAttachResize();
	void SetAttachBones(const unsigned int vertexId, const unsigned int boneId);
	void SetAttachWeight(const unsigned int vertexId, const float weight);
	void SetNumWeights(const std::vector<unsigned int>& numWeights);


	void IncrementAttachNumBones(const unsigned int vertexId);

	const std::vector<Triangle> RetrieveTriangles(const float4x4& modelMatrix);

protected:
	void InternalLoad() override;
	void InternalUnload() override;

private:
	void CreateVBO();
	void CreateEBO();
	void CreateVAO();

	static constexpr unsigned int bonesPerVertex = 4;

	unsigned int vbo;
	unsigned int ebo;
	unsigned int vao;

	//parameters for buffer object creation
	unsigned int numVertices;
	unsigned int numFaces;
	unsigned int numIndexes;
	unsigned int numBones;
	unsigned int materialIndex;
	std::vector<float3> vertices;
	std::vector<float3> textureCoords;
	std::vector<float3> normals;
	std::vector<float3> tangents{};
	std::vector<std::vector<unsigned int> > facesIndices;
	std::vector<Bone> bones;
	std::vector<Attach> attaches;
	std::vector<unsigned int> numWeights;

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

inline unsigned int ResourceMesh::GetNumBones()
{
	return numBones;
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

inline const std::vector<Bone>& ResourceMesh::GetBones() const 
{
	return bones;
}

inline const std::vector<Attach>& ResourceMesh::GetAttaches() const 
{
	return attaches;
}

inline const std::vector<unsigned int>& ResourceMesh::GetNumWeights() const
{
	return this->numWeights;
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

inline void ResourceMesh::SetNumBones(unsigned int numBones)
{
	this->numBones = numBones;
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

inline void ResourceMesh::SetBones(const std::vector<Bone>& bones)
{
	this->bones = bones;
}

inline void ResourceMesh::SetAttachResize()
{
	this->attaches.resize(this->numVertices);
}

inline void ResourceMesh::SetAttachBones(const unsigned int vertexId, const unsigned int boneId)
{
	this->attaches[vertexId].bones[this->attaches[vertexId].numBones] = boneId;
}

inline void ResourceMesh::SetAttachWeight(const unsigned int vertexId, const float weight)
{
	this->attaches[vertexId].weights[this->attaches[vertexId].numBones] = weight;
}

inline void ResourceMesh::SetNumWeights(const std::vector<unsigned int>& numWeights)
{
	this->numWeights = numWeights;
}

inline void ResourceMesh::IncrementAttachNumBones(const unsigned int vertexId)
{
	if (this->attaches[vertexId].numBones < bonesPerVertex)
	{
		++this->attaches[vertexId].numBones;
	}
}
