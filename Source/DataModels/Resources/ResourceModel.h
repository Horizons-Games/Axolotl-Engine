#pragma once

#include "Resource.h"

#include <vector>

#include "FileSystem/UniqueID.h"

class ResourceModel : public Resource
{
public:
	ResourceModel(UID resourceUID, const std::string& fileName, const std::string& assetsPath, const std::string& libraryPath);
	~ResourceModel() override = default;

	ResourceType GetType() const override;

	void Load() override {};
	void Unload() override {};

	const unsigned int GetNumMeshes() const;
	const unsigned int GetNumTextures() const;
	const std::vector<UID>& GetMeshesUIDs() const;
	const std::vector<UID>& GetTexturesUIDs() const;

	void SetNumMeshes(const unsigned int numMeshes);
	void SetNumTextures(const unsigned int numTextures);
	void SetMeshesUIDs(const std::vector<UID>& meshesUIDs);
	void SetTexturesUIDs(const std::vector<UID>& texturesUIDs);

private:
	unsigned int numMeshes;
	unsigned int numTextures;
	std::vector<UID> meshesUIDs;
	std::vector<UID> texturesUIDs;
};

inline ResourceModel::ResourceModel(UID resourceUID,
									const std::string& fileName,
									const std::string& assetsPath,
									const std::string& libraryPath) :
	Resource(resourceUID, fileName, assetsPath, libraryPath)
{
}

inline ResourceType ResourceModel::GetType() const
{
	return ResourceType::Model;
}

inline const std::vector<UID>& ResourceModel::GetMeshesUIDs() const
{
	return meshesUIDs;
}

inline const std::vector<UID>& ResourceModel::GetTexturesUIDs() const
{
	return texturesUIDs;
}

inline void ResourceModel::SetMeshesUIDs(const std::vector<UID>& meshesUIDs)
{
	this->meshesUIDs = meshesUIDs;
	this->numMeshes = meshesUIDs.size();
}

inline void ResourceModel::SetTexturesUIDs(const std::vector<UID>& texturesUIDs)
{
	this->texturesUIDs = texturesUIDs;
	this->numTextures = texturesUIDs.size();
}
