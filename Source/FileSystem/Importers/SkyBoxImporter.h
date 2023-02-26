#pragma once
#include "FileSystem/Importers/Importer.h"
#include "DataModels/Resources/ResourceSkyBox.h"

class SkyBoxImporter : public Importer<ResourceSkyBox>
{
public:
    SkyBoxImporter();
    ~SkyBoxImporter();

    void Import(const char* filePath, std::shared_ptr<ResourceSkyBox> resource) override;
    void Load(const char* fileBuffer, std::shared_ptr<ResourceSkyBox> resource) override;

protected:
    void Save(const std::shared_ptr<ResourceSkyBox>& resource, char*& fileBuffer, unsigned int& size) override;
};
