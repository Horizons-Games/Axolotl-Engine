#pragma once

#include "Resource.h"

#include <memory>
#include <vector>

struct OptionsModel
{
};

class ResourceModel : virtual public Resource
{
public:
	ResourceModel(UID resourceUID, const std::string& fileName, const std::string& assetsPath, const std::string& libraryPath);
	virtual ~ResourceModel() override;

	ResourceType GetType() const override;

	void SaveOptions(Json& meta) override {};
	void LoadOptions(Json& meta) override {};

	const size_t GetNumMeshes() const;
	const size_t GetNumMaterials() const;
	const std::vector<UID>& GetMeshesUIDs() const;
	const std::vector<UID>& GetMaterialsUIDs() const;

	std::shared_ptr<OptionsModel>& GetOptions();

	void SetNumMeshes(const unsigned int numMeshes);
	void SetNumMaterials(const unsigned int numMaterials);
	void SetMeshesUIDs(const std::vector<UID>& meshesUIDs);
	void SetMaterialsUIDs(const std::vector<UID>& materialsUIDs);

protected:
	void InternalLoad() override;
	void InternalUnload() override;

private:
	size_t numMeshes;
	size_t numMaterials;
	std::vector<UID> meshesUIDs;
	std::vector<UID> materialsUIDs;

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

inline ResourceModel::~ResourceModel()
{
	this->Unload();
}

inline ResourceType ResourceModel::GetType() const
{
	return ResourceType::Model;
}

inline const size_t ResourceModel::GetNumMeshes() const
{
	return numMeshes;
}

inline const size_t ResourceModel::GetNumMaterials() const
{
	return numMaterials;
}

inline const std::vector<UID>& ResourceModel::GetMeshesUIDs() const
{
	return meshesUIDs;
}

inline const std::vector<UID>& ResourceModel::GetMaterialsUIDs() const
{
	return materialsUIDs;
}

inline std::shared_ptr<OptionsModel>& ResourceModel::GetOptions()
{
	this->options;
}

inline void ResourceModel::SetNumMeshes(const unsigned int numMeshes)
{
	this->numMeshes = numMeshes;
}

inline void ResourceModel::SetNumMaterials(const unsigned int numMaterials)
{
	this->numMaterials = numMaterials;
}

inline void ResourceModel::SetMeshesUIDs(const std::vector<UID>& meshesUIDs)
{
	this->meshesUIDs = meshesUIDs;
	this->numMeshes = meshesUIDs.size();
}

inline void ResourceModel::SetMaterialsUIDs(const std::vector<UID>& materialsUIDs)
{
	this->materialsUIDs = materialsUIDs;
	this->numMaterials = materialsUIDs.size();
}
