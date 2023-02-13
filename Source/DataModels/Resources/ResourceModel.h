#pragma once

#include "Resource.h"

#include <memory>
#include <vector>

struct OptionsModel
{
};

class ResourceModel : public Resource
{
public:
	ResourceModel(UID resourceUID, const std::string& fileName, const std::string& assetsPath, const std::string& libraryPath);
	~ResourceModel() override;

	ResourceType GetType() const override;

	void SaveOptions(Json& meta) override {};
	void LoadOptions(Json& meta) override {};

	const size_t GetNumMeshes() const;
	const size_t GetNumMaterials() const;
	const std::vector<std::shared_ptr<Resource>>& GetMeshes() const;
	const std::vector<std::shared_ptr<Resource>>& GetMaterials() const;

	std::shared_ptr<OptionsModel>& GetOptions();

	void SetNumMeshes(const unsigned int numMeshes);
	void SetNumMaterials(const unsigned int numMaterials);
	void SetMeshes(const std::vector<std::shared_ptr<Resource>>& meshes);
	void SetMaterials(const std::vector<std::shared_ptr<Resource>>& materials);

protected:
	void InternalLoad() override;
	void InternalUnload() override;

private:
	size_t numMeshes;
	size_t numMaterials;

	std::vector<std::shared_ptr<Resource>> meshes;
	std::vector<std::shared_ptr<Resource>> materials;

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

inline const std::vector<std::shared_ptr<Resource>>& ResourceModel::GetMeshes() const
{
	return meshes;
}

inline const std::vector<std::shared_ptr<Resource>>& ResourceModel::GetMaterials() const
{
	return materials;
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

inline void ResourceModel::SetMeshes(const std::vector<std::shared_ptr<Resource>>& meshes)
{
	this->meshes = meshes;
	this->numMeshes = meshes.size();
}

inline void ResourceModel::SetMaterials(const std::vector<std::shared_ptr<Resource>>& materials)
{
	this->materials = materials;
	this->numMaterials = materials.size();
}
