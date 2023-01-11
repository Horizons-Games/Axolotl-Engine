#pragma once

#include "Resource.h"
#include <memory>

struct OptionsScene
{
};

class ResourceScene : public Resource
{
public:
	ResourceScene(UID resourceUID, const std::string& fileName, const std::string& assetsPath, const std::string& libraryPath);
	~ResourceScene() override = default;

	ResourceType GetType() const override;

	void Load() override {};
	void Unload() override {};
	void SaveOptions(Json& meta) override {};
	void LoadOptions(Json& meta) override {};

	//Gets

	std::shared_ptr<OptionsScene>& GetOptions();

	//Sets
private:


	std::shared_ptr<OptionsScene> options;
};

inline ResourceScene::ResourceScene(UID resourceUID,
	const std::string& fileName,
	const std::string& assetsPath,
	const std::string& libraryPath) :
	Resource(resourceUID, fileName, assetsPath, libraryPath)
{
	options = std::make_shared<OptionsScene>();
}

inline ResourceType ResourceScene::GetType() const
{
	return ResourceType::Scene;
}

inline std::shared_ptr<OptionsScene>& ResourceScene::GetOptions()
{
	this->options;
}
