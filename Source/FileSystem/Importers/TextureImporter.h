#pragma once
#include "FileSystem/Importers/Importer.h"
#include "DataModels/Resources/ResourceTexture.h"

class TextureImporter : public Importer<ResourceTexture>
{
public:
    TextureImporter() = default;
    ~TextureImporter();

    void Import(const char* filePath, std::shared_ptr<ResourceTexture> resource) override;
    uint64_t Save(const std::shared_ptr<ResourceTexture> ourTexture, char*& fileBuffer) override;
    void Load(const char* fileBuffer, std::shared_ptr<ResourceTexture> ourTexture) override;

private:
    char* buffer;
};

inline TextureImporter::~TextureImporter()
{
    delete[] buffer;
}

