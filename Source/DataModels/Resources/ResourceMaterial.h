#pragma once

#include "Resource.h"
#include <memory>

struct OptionsMaterial
{
};

class ResourceMaterial : public Resource
{
public:
	ResourceMaterial(UID resourceUID, const std::string& fileName, const std::string& assetsPath, const std::string& libraryPath);
	~ResourceMaterial() override = default;

	ResourceType GetType() const override;

	void Load() override {};
	void Unload() override {};
	void SaveOptions(Json& meta) override {};
	void LoadOptions(Json& meta) override {};

	//Gets

	std::shared_ptr<OptionsMaterial>& GetOptions();

	//Sets
private:


	std::shared_ptr<OptionsMaterial> options;
};

inline ResourceMaterial::ResourceMaterial(UID resourceUID,
	const std::string& fileName,
	const std::string& assetsPath,
	const std::string& libraryPath) :
	Resource(resourceUID, fileName, assetsPath, libraryPath)
{
	options = std::make_shared<OptionsMaterial>();
}

inline ResourceType ResourceMaterial::GetType() const
{
	return ResourceType::Material;
}

inline std::shared_ptr<OptionsMaterial>& ResourceMaterial::GetOptions()
{
	this->options;
}
