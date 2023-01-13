#pragma once

#include "Resource.h"

#include "Math/float3.h"

#include <memory>


struct OptionsMesh
{
};

class ResourceMesh : public Resource
{
public:
	ResourceMesh(UID resourceUID, const std::string& fileName, const std::string& assetsPath, const std::string& libraryPath);
	~ResourceMesh() override = default;

	ResourceType GetType() const override;

	void Load() override;
	void Unload() override;
	void SaveOptions(Json& meta) override {};
	void LoadOptions(Json& meta) override {};

	unsigned int GetVBO() const;
	unsigned int GetEBO() const;
	unsigned int GetVAO() const;

	unsigned int GetNumVertices();
	unsigned int GetNumFaces();
	unsigned int GetNumIndexes();
	unsigned int GetMaterialIndex();
	const std::vector<float3>& GetVertices();
	const std::vector<float3>& GetTextureCoords();
	const std::vector<std::vector<unsigned int> >& GetFacesIndices();

	std::shared_ptr<OptionsMesh>& GetOptions();

	void SetNumVertices(unsigned int numVertices);
	void SetNumFaces(unsigned int numFaces);
	void SetNumIndexes(unsigned int numIndexes);
	void SetMaterialIndex(unsigned int materialIndex);
	void SetVertices(const std::vector<float3>& vertices);
	void SetTextureCoords(const std::vector<float3>& textureCoords);
	void SetFacesIndices(const std::vector<std::vector<unsigned int> >& facesIndices);

private:
	void CreateVBO();
	void CreateEBO();
	void CreateVAO();

	unsigned int vbo = 0;
	unsigned int ebo = 0;
	unsigned int vao = 0;

	//parameters for buffer object creation
	unsigned int numVertices = 0;
	unsigned int numFaces = 0;
	unsigned int numIndexes = 0;
	unsigned int materialIndex = 0;
	std::vector<float3> vertices;
	std::vector<float3> textureCoords;
	std::vector<std::vector<unsigned int> > facesIndices;

	std::shared_ptr<OptionsMesh> options;
};

inline ResourceMesh::ResourceMesh(UID resourceUID,
								  const std::string& fileName,
								  const std::string& assetsPath,
								  const std::string& libraryPath) :
	Resource(resourceUID, fileName, assetsPath, libraryPath)
{
	options = std::make_shared<OptionsMesh>();
}

inline ResourceType ResourceMesh::GetType() const
{
	return ResourceType::Mesh;
}

inline unsigned int ResourceMesh::GetVBO() const
{
	return this->vbo;
}

inline unsigned int ResourceMesh::GetEBO() const
{
	return this->ebo;
}

inline unsigned int ResourceMesh::GetVAO() const
{
	return this->vao;
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

inline const std::vector<std::vector<unsigned int> >& ResourceMesh::GetFacesIndices()
{
	return facesIndices;
}

inline std::shared_ptr<OptionsMesh>& ResourceMesh::GetOptions()
{
	return this->options;
}

inline void ResourceMesh::SetNumVertices(unsigned int numVertices)
{
	this->numVertices = numVertices;
}

inline void ResourceMesh::SetNumFaces(unsigned int numFaces)
{
	this->numFaces = numFaces;
	this->numIndexes = numFaces * 3;
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

inline void ResourceMesh::SetFacesIndices(const std::vector<std::vector<unsigned int> >& facesIndices)
{
	this->facesIndices = facesIndices;
}
