#pragma once

#include "Resource.h"

class ResourceMesh : public Resource
{
public:
	ResourceMesh(UID resourceUID, const std::string& assetsPath, const std::string& libraryPath);
	~ResourceMesh() override = default;

	ResourceType GetType() const override;

	unsigned int GetVBO() const;
	unsigned int GetEBO() const;
	unsigned int GetVAO() const;

private:
	void CreateVBO();
	void CreateEBO();
	void CreateVAO();

	unsigned int vbo = 0;
	unsigned int ebo = 0;
	unsigned int vao = 0;
};

inline ResourceMesh::ResourceMesh(UID resourceUID,
								  const std::string& assetsPath,
								  const std::string& libraryPath) :
	Resource(resourceUID, assetsPath, libraryPath)
{
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
