#pragma once

#include "Resource.h"

#include <vector>

#include "FileSystem/UniqueID.h"

struct OptionsModel
{
};

class ResourceModel : public Resource
{
public:
	ResourceModel(UID resourceUID, const std::string& fileName, const std::string& assetsPath, const std::string& libraryPath);
	~ResourceModel() override = default;

	ResourceType GetType() const override;

	void Load() override {};
	void Unload() override {};
	void SaveOptions(Json& meta) override {};
	void LoadOptions(Json& meta) override {};

	const unsigned int GetNumMeshes() const;
	const unsigned int GetNumTextures() const;
	const std::vector<UID>& GetMeshesUIDs() const;
	const std::vector<UID>& GetTexturesUIDs() const;

	std::shared_ptr<OptionsModel>& GetOptions();

	void SetNumMeshes(const unsigned int numMeshes);
	void SetNumTextures(const unsigned int numTextures);
	void SetMeshesUIDs(const std::vector<UID>& meshesUIDs);
	void SetTexturesUIDs(const std::vector<UID>& texturesUIDs);

private:
	unsigned int numMeshes;
	unsigned int numTextures;
	std::vector<UID> meshesUIDs;
	std::vector<UID> texturesUIDs;

	std::shared_ptr<OptionsModel> options;
};

inline ResourceModel::ResourceModel(UID resourceUID,
									const std::string& fileName,
									const std::string& assetsPath,
									const std::string& libraryPath) :
	Resource(resourceUID, fileName, assetsPath, libraryPath)
{
	options = std::make_shared<OptionsModel>();
}

inline ResourceType ResourceModel::GetType() const
{
	return ResourceType::Model;
}

inline const unsigned int ResourceModel::GetNumMeshes() const
{
	return numMeshes;
}

inline const unsigned int ResourceModel::GetNumTextures() const
{
	return numTextures;
}

inline const std::vector<UID>& ResourceModel::GetMeshesUIDs() const
{
	return meshesUIDs;
}

inline const std::vector<UID>& ResourceModel::GetTexturesUIDs() const
{
	return texturesUIDs;
}

inline std::shared_ptr<OptionsModel>& ResourceModel::GetOptions()
{
	this->options;
}

inline void ResourceModel::SetNumMeshes(const unsigned int numMeshes)
{
	this->numMeshes = numMeshes;
}

inline void ResourceModel::SetNumTextures(const unsigned int numTextures)
{
	this->numTextures = numTextures;
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
