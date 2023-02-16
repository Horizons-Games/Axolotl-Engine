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
	~ResourceScene() override;

	ResourceType GetType() const override;

	void SaveOptions(Json& meta) override {};
	void LoadOptions(Json& meta) override {};

	//Getters

	std::shared_ptr<OptionsScene>& GetOptions();

	//Setters

protected:
	void InternalLoad() override {};
	void InternalUnload() override {};

private:
	std::shared_ptr<OptionsScene> options;
};

inline ResourceType ResourceScene::GetType() const
{
	return ResourceType::Scene;
}

inline std::shared_ptr<OptionsScene>& ResourceScene::GetOptions()
{
	this->options;
}
