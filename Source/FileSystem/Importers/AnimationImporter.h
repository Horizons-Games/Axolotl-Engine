#pragma once

#include "FileSystem/Importers/Importer.h"
#include "Resources/ResourceAnimation.h"

class AnimationImporter : public Importer<ResourceAnimation>
{
public:
	AnimationImporter(){};
	~AnimationImporter(){};

	void Import(const char* filePath, std::shared_ptr<ResourceAnimation> resource) override;
	void Load(const char* fileBuffer, std::shared_ptr<ResourceAnimation> resource) override;

protected:
	void Save(const std::shared_ptr<ResourceAnimation>& resource, char*& fileBuffer, unsigned int& size) override;
};
