#pragma once

#include "Resource.h"
#include <memory>

struct OptionsSkyBox
{
};

class ResourceSkyBox : public Resource
{
public:
	ResourceSkyBox(UID resourceUID, const std::string& fileName, const std::string& assetsPath, const std::string& libraryPath);
	~ResourceSkyBox() override = default;

	ResourceType GetType() const override;

	void Load() override;
	void Unload() override;
	void SaveOptions(Json& meta) override {};
	void LoadOptions(Json& meta) override {};

	//Gets
	std::shared_ptr<OptionsSkyBox>& GetOptions();

	//Sets
private:
	unsigned int glTexture = 0;
	std::vector<UID> texturesUIDs;
	std::shared_ptr<OptionsSkyBox> options;
};

inline ResourceSkyBox::ResourceSkyBox(UID resourceUID,
									  const std::string& fileName,
									  const std::string& assetsPath,
									  const std::string& libraryPath) :
	Resource(resourceUID, fileName, assetsPath, libraryPath),
	options(std::make_shared<OptionsSkyBox>())
{
}

inline ResourceType ResourceSkyBox::GetType() const
{
	return ResourceType::SkyBox;
}

inline std::shared_ptr<OptionsSkyBox>& ResourceSkyBox::GetOptions()
{
	return this->options;
}
