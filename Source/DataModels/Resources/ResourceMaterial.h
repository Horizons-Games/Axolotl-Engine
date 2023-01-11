#pragma once

#include "Resource.h"
#include <memory>
#include <string>
#include "Math/float3.h"

struct Texture
{
	unsigned id = 0;
	std::string path;
	unsigned width = 0;
	unsigned height = 0;
};

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

	void bind(unsigned int program);

	//Gets
	unsigned GetDiffuseId();
	unsigned GetSpecularId();

	std::shared_ptr<OptionsMaterial>& GetOptions();

	//Sets
private:

	Texture diffuse;
	Texture specular;
	float3 diffuse_color = float3(1.0);
	float3 specular_color = float3(0.5, 0.3, 0.5);
	float shininess;

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
inline unsigned ResourceMaterial::GetDiffuseId()
{
	return diffuse.id;
}
inline unsigned ResourceMaterial::GetSpecularId()
{
	return specular.id;
}