#pragma once

#include "Resource.h"
#include <memory>

struct OptionsScene
{
};

class ResourceScene : virtual public Resource
{
public:
	ResourceScene(UID resourceUID,
				  const std::string& fileName,
				  const std::string& assetsPath,
				  const std::string& libraryPath);
	virtual ~ResourceScene() override;

	ResourceType GetType() const override;

	void SaveImporterOptions(Json& meta) override{};
	void LoadImporterOptions(Json& meta) override{};

	OptionsScene& GetOptions();

protected:
	void InternalLoad() override{};
	void InternalUnload() override{};

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
