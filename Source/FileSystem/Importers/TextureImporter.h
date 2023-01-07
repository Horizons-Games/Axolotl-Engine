#pragma once
#include "FileSystem/Importers/Importer.h"
#include "FileSystem/Data.h"


class TextureImporter : public Importer<void, DataTexture>
{
public:
    TextureImporter() = default;
    ~TextureImporter();

    void Start(const char* filePath, DataTexture* ourTexture) override;
    uint64_t Save(const DataTexture* ourTexture, char*& fileBuffer) override;
    void Load(const char* fileBuffer, DataTexture* ourTexture) override;

private:
    char* buffer;
};

inline TextureImporter::~TextureImporter()
{
    delete[] buffer;
}

