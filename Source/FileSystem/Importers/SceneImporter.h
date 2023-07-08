#pragma once
#include "DataModels/Resources/ResourceScene.h"
#include "FileSystem/Importers/Importer.h"

class SceneImporter : public Importer<ResourceScene>
{
public:
	SceneImporter();
	~SceneImporter() override;

	void Import(const char* filePath, std::shared_ptr<ResourceScene> resource) override;
	void Load(const char* fileBuffer, std::shared_ptr<ResourceScene> resource) override;

protected:
	void Save(const std::shared_ptr<ResourceScene>& resource, char*& fileBuffer, unsigned int& size) override;
};
