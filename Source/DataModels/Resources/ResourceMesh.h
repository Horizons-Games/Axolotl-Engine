#pragma once
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
	unsigned int bones[4] = { 0u, 0u, 0u, 0u };
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
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

	void SaveImporterOptions(Json& meta) override{};
	void LoadImporterOptions(Json& meta) override{};

	void SaveLoadOptions(Json& meta) override{};
	void LoadLoadOptions(Json& meta) override{};

	unsigned int GetVBO() const;
	unsigned int GetEBO() const;
	unsigned int GetVAO() const;

	unsigned int GetNumVertices() const;
	unsigned int GetNumFaces() const;
	unsigned int GetNumIndexes() const;
	unsigned int GetNumBones() const;
	unsigned int GetMaterialIndex() const;
	unsigned int GetSSBOPalette() const;
	unsigned int GetNumBonesAttached(const unsigned int vertexId) const;
	const std::vector<float3>& GetVertices() const;
	const std::vector<float3>& GetTextureCoords() const;
	const std::vector<float3>& GetNormals() const;
	const std::vector<float3>& GetTangents() const;
	const std::vector<std::vector<unsigned int>>& GetFacesIndices() const;
	const std::vector<Bone>& GetBones() const;
	const std::vector<Attach>& GetAttaches() const;
	const std::vector<unsigned int>& GetNumWeights() const;
	unsigned int GetBonesPerVertex();

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
	void SetFacesIndices(const std::vector<std::vector<unsigned int>>& facesIndices);
	void SetBones(const std::vector<Bone>& bones);
	void SetAttachResize();
	void SetAttachBones(const unsigned int vertexId, const unsigned int boneId);
	void SetAttachWeight(const unsigned int vertexId, const float weight);
	void SetNumWeights(const std::vector<unsigned int>& numWeights);

	void IncrementAttachNumBones(const unsigned int vertexId);
	void NormalizeWeights(const unsigned int attachId);

	const std::vector<Triangle> RetrieveTriangles(const float4x4& modelMatrix);

protected:
	void InternalLoad() override;
	void InternalUnload() override;

private:
	void CreateVBO();
	void CreateEBO();
	void CreateVAO();

	/* Due to GPU limitations, the number of bones influencing a single vertex
	must be limited. The most widespread limitation is 4 bones per vertex.*/
	static constexpr unsigned int bonesPerVertex = 4;

	unsigned int vbo;
	unsigned int ebo;
	unsigned int vao;

	// parameters for buffer object creation
	unsigned int numVertices;
	unsigned int numFaces;
	unsigned int numIndexes;
	unsigned int numBones;
	unsigned int materialIndex;
	unsigned int ssboPalette;

	std::vector<float3> vertices;
	std::vector<float3> textureCoords;
	std::vector<float3> normals;
	std::vector<float3> tangents;
	std::vector<std::vector<unsigned int>> facesIndices;
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

inline unsigned int ResourceMesh::GetNumVertices() const
{
	return numVertices;
}

inline unsigned int ResourceMesh::GetNumFaces() const
{
	return numFaces;
}

inline unsigned int ResourceMesh::GetNumIndexes() const
{
	return numIndexes;
}

inline unsigned int ResourceMesh::GetNumBones() const
{
	return numBones;
}

inline unsigned int ResourceMesh::GetMaterialIndex() const
{
	return materialIndex;
}

inline unsigned int ResourceMesh::GetSSBOPalette() const
{
	return ssboPalette;
}

inline const std::vector<float3>& ResourceMesh::GetVertices() const
{
	return vertices;
}

inline const std::vector<float3>& ResourceMesh::GetTextureCoords() const
{
	return textureCoords;
}

inline const std::vector<float3>& ResourceMesh::GetNormals() const
{
	return normals;
}

inline const std::vector<float3>& ResourceMesh::GetTangents() const
{
	return tangents;
}

inline const std::vector<std::vector<unsigned int>>& ResourceMesh::GetFacesIndices() const
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

inline unsigned int ResourceMesh::GetNumBonesAttached(const unsigned int vertexId) const
{
	return attaches[vertexId].numBones;
}

inline const std::vector<unsigned int>& ResourceMesh::GetNumWeights() const
{
	return numWeights;
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

inline void ResourceMesh::SetFacesIndices(const std::vector<std::vector<unsigned int>>& facesIndices)
{
	this->facesIndices = facesIndices;
}

inline void ResourceMesh::SetBones(const std::vector<Bone>& bones)
{
	this->bones = bones;
}

inline void ResourceMesh::SetAttachResize()
{
	attaches.resize(numVertices);
}

inline void ResourceMesh::SetAttachBones(const unsigned int vertexId, const unsigned int boneId)
{
	attaches[vertexId].bones[attaches[vertexId].numBones] = boneId;
}

inline void ResourceMesh::SetAttachWeight(const unsigned int vertexId, const float weight)
{
	attaches[vertexId].weights[attaches[vertexId].numBones] = weight;
}

inline void ResourceMesh::SetNumWeights(const std::vector<unsigned int>& numWeights)
{
	this->numWeights = numWeights;
}

inline void ResourceMesh::IncrementAttachNumBones(const unsigned int vertexId)
{
	if (attaches[vertexId].numBones < bonesPerVertex)
	{
		++attaches[vertexId].numBones;
	}
}

inline void ResourceMesh::NormalizeWeights(const unsigned int attachId)
{
	float totalWeight = 0.0f;

	for (unsigned int i = 0; i < bonesPerVertex; ++i)
	{
		totalWeight += attaches[attachId].weights[i];
	}

	if (totalWeight != 1.0f)
	{
		for (unsigned int i = 0; i < bonesPerVertex; ++i)
		{
			attaches[attachId].weights[i] = attaches[attachId].weights[i] / totalWeight;
		}
	}
}
