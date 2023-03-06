#pragma once

#include "Resource.h"
#include <memory>

struct OptionsScene
{
};

class ResourceScene : virtual public Resource
{
public:
	ResourceScene(UID resourceUID, const std::string& fileName, const std::string& assetsPath, const std::string& libraryPath);
	virtual ~ResourceScene() override;

	ResourceType GetType() const override;

	void SaveOptions(Json& meta) override {};
	void LoadOptions(Json& meta) override {};

	//Getters

	OptionsScene& GetOptions();

	//Setters

protected:
	void InternalLoad() override {};
	void InternalUnload() override {};

private:
	OptionsScene options;
};

inline ResourceType ResourceScene::GetType() const
{
	return ResourceType::Scene;
}

inline OptionsScene& ResourceScene::GetOptions()
{
	return options;
}
