#pragma once

#include "Resource.h"

#include <memory>

class ResourceMesh;
class ResourceMaterial;

struct OptionsModel
{
};

class ResourceModel : virtual public Resource
{
public:
	ResourceModel(UID resourceUID, 
		const std::string& fileName, 
		const std::string& assetsPath, 
		const std::string& libraryPath);
	virtual ~ResourceModel() override;

	ResourceType GetType() const override;

	void SaveOptions(Json& meta) override {};
	void LoadOptions(Json& meta) override {};

	const size_t GetNumMeshes() const;
	const size_t GetNumMaterials() const;
	const std::vector<std::shared_ptr<ResourceMesh>>& GetMeshes() const;
	const std::vector<std::shared_ptr<ResourceMaterial>>& GetMaterials() const;

	std::shared_ptr<OptionsModel>& GetOptions();

	void SetNumMeshes(const unsigned int numMeshes);
	void SetNumMaterials(const unsigned int numMaterials);
	void SetMeshes(const std::vector<std::shared_ptr<ResourceMesh>>& meshes);
	void SetMaterials(const std::vector<std::shared_ptr<ResourceMaterial>>& materials);

protected:
	void InternalLoad() override;
	void InternalUnload() override;

private:
	size_t numMeshes;
	size_t numMaterials;

	std::vector<std::shared_ptr<ResourceMesh>> meshes;
	std::vector<std::shared_ptr<ResourceMaterial>> materials;

	std::shared_ptr<OptionsModel> options;
};

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

inline const std::vector<std::shared_ptr<ResourceMesh>>& ResourceModel::GetMeshes() const
{
	return meshes;
}

inline const std::vector<std::shared_ptr<ResourceMaterial>>& ResourceModel::GetMaterials() const
{
	return materials;
}

inline std::shared_ptr<OptionsModel>& ResourceModel::GetOptions()
{
	return options;
}

inline void ResourceModel::SetNumMeshes(const unsigned int numMeshes)
{
	this->numMeshes = numMeshes;
}

inline void ResourceModel::SetNumMaterials(const unsigned int numMaterials)
{
	this->numMaterials = numMaterials;
}

inline void ResourceModel::SetMeshes(const std::vector<std::shared_ptr<ResourceMesh>>& meshes)
{
	this->meshes = meshes;
	this->numMeshes = meshes.size();
}

inline void ResourceModel::SetMaterials(const std::vector<std::shared_ptr<ResourceMaterial>>& materials)
{
	this->materials = materials;
	this->numMaterials = materials.size();
}
