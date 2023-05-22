#pragma once
#include "FileSystem/Importers/Importer.h"

class ResourceCubemap;

class CubemapImporter : public Importer<ResourceCubemap>
{
public:
	CubemapImporter();
	~CubemapImporter() override;

	void Import(const char* filePath, std::shared_ptr<ResourceCubemap> resource) override;
	void Load(const char* fileBuffer, std::shared_ptr<ResourceCubemap> resource) override;

protected:
	void Save(const std::shared_ptr<ResourceCubemap>& resource, char*& fileBuffer, unsigned int& size) override;
};
