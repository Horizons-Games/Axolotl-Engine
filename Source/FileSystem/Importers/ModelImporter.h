#pragma once
#include "FileSystem/Importers/Importer.h"
#include "FileSystem/Data.h"

class aiScene;
class aiMesh;

class ModelImporter : public Importer<DataModel>
{
public:
    ModelImporter() = default;
    ~ModelImporter();

    void Import(const char* filePath, std::shared_ptr<DataModel> resource) override;
    uint64_t Save(const std::shared_ptr<DataModel> resource, char*& fileBuffer) override;
    void Load(const char* fileBuffer, std::shared_ptr<DataModel> resource) override;

private:

    void ImportMaterials(const aiScene* scene, const char* filePath, std::shared_ptr<DataModel>& resource);
    void ImportMeshes(const aiScene* scene, const char* filePath, std::shared_ptr<DataModel>& resource);

    void SaveInfoMesh(const aiMesh* ourMesh, char*& fileBuffer, unsigned int& size);

    char* buffer;
};

inline ModelImporter::~ModelImporter()
{
    delete[] buffer;
}

