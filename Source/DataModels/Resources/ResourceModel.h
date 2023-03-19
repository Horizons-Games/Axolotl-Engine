#pragma once

#include "Resource.h"

#include "Components/ComponentMeshRenderer.h"

#include "Math/float4x4.h"

#include <memory>

class ResourceMesh;
class ResourceMaterial;

struct OptionsModel
{
};

class ResourceModel : virtual public Resource
{
public:
	struct Node
	{
		char* name;
		float4x4 transform;
		Node* parent;
		std::vector<ComponentMeshRenderer> meshRenderers;
	};

public:
	ResourceModel(UID resourceUID, 
		const std::string& fileName, 
		const std::string& assetsPath, 
		const std::string& libraryPath);
	virtual ~ResourceModel() override;

	ResourceType GetType() const override;

	void SaveImporterOptions(Json& meta) override {};
	void LoadImporterOptions(Json& meta) override {};

	const size_t GetNumMeshes() const;
	const size_t GetNumMaterials() const;
	const std::vector<std::shared_ptr<ResourceMesh>>& GetMeshes() const;
	const std::vector<std::shared_ptr<ResourceMaterial>>& GetMaterials() const;

	OptionsModel& GetOptions();

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

	OptionsModel options;

	std::vector<Node> nodes;
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

inline OptionsModel& ResourceModel::GetOptions()
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
