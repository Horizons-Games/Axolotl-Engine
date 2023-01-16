#pragma once
#include "FileSystem/Importers/Importer.h"
#include "DataModels/Resources/ResourceTexture.h"

class TextureImporter : public Importer<ResourceTexture>
{
public:
    TextureImporter() = default;
    ~TextureImporter() = default;

    void Import(const char* filePath, std::shared_ptr<ResourceTexture> resource) override;
    void Load(const char* fileBuffer, std::shared_ptr<ResourceTexture> resource) override;

protected:
    void Save(const std::shared_ptr<ResourceTexture>& resource, char*& fileBuffer, unsigned int& size) override;
};
