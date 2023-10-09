#pragma once
#include "FileSystem/Importers/Importer.h"
#include <memory>

class ResourceVideo;

class VideoImporter : public Importer<ResourceVideo>
{
public:
	VideoImporter();
	~VideoImporter() override;

	void Import(const char* filePath, std::shared_ptr<ResourceVideo> resource) override;
	void Load(const char* fileBuffer, std::shared_ptr<ResourceVideo> resource) override;

protected:
	void Save(const std::shared_ptr<ResourceVideo>& resource, char*& fileBuffer, unsigned int& size) override;
};
