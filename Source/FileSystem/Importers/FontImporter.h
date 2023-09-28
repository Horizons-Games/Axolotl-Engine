#pragma once
#include "FileSystem/Importers/Importer.h"
class ResourceFont;

class FontImporter : public Importer<ResourceFont>
{
public:
    FontImporter();
    ~FontImporter() override;

    void Import(const char* filePath, std::shared_ptr<ResourceFont> resource) override;
    void Load(const char* fileBuffer, std::shared_ptr<ResourceFont> resource) override;

protected:
    void Save(const std::shared_ptr<ResourceFont>& resource, char*& fileBuffer, unsigned int& size) override;
};
