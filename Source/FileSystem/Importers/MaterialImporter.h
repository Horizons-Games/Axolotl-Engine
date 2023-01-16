#pragma once
#include "FileSystem/Importers/Importer.h"
#include "DataModels/Resources/ResourceMaterial.h"

class MaterialImporter : public Importer<ResourceMaterial>
{
public:
    MaterialImporter() = default;
    ~MaterialImporter() = default;

    void Import(const char* filePath, std::shared_ptr<ResourceMaterial> resource) override;
    void Load(const char* fileBuffer, std::shared_ptr<ResourceMaterial> resource) override;

protected:
    uint64_t Save(const std::shared_ptr<ResourceMaterial>& resource, char*& fileBuffer, unsigned int& size) override;
};
