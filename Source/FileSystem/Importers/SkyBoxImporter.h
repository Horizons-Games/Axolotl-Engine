#pragma once
#include "FileSystem/Importers/Importer.h"
#include "DataModels/Resources/ResourceSkyBox.h"

class SkyBoxImporter : public Importer<ResourceSkyBox>
{
public:
    SkyBoxImporter() = default;
    ~SkyBoxImporter();

    void Import(const char* filePath, std::shared_ptr<ResourceSkyBox> resource) override;

protected:
    uint64_t Save(const std::shared_ptr<ResourceSkyBox>& resource, char*& fileBuffer, unsigned int& size) override;
    void Load(const char* fileBuffer, std::shared_ptr<ResourceSkyBox>& resource) override;

private:
    char* buffer;
};

inline SkyBoxImporter::~SkyBoxImporter()
{
    delete[] buffer;
}

