#pragma once
#include "FileSystem/Importers/Importer.h"
#include "DataModels/Resources/ResourceModel.h"

class aiScene;
class aiMesh;

class ModelImporter : public Importer<ResourceModel>
{
public:
    ModelImporter() = default;
    ~ModelImporter();

    void Import(const char* filePath, std::shared_ptr<ResourceModel> resource) override;
    uint64_t Save(const std::shared_ptr<ResourceModel> resource, char*& fileBuffer) override;
    void Load(const char* fileBuffer, std::shared_ptr<ResourceModel> resource) override;

private:

    void ImportMaterials(const aiScene* scene, const char* filePath, std::shared_ptr<ResourceModel>& resource);
    void ImportMeshes(const aiScene* scene, const char* filePath, std::shared_ptr<ResourceModel>& resource);

    void SaveInfoMesh(const aiMesh* ourMesh, char*& fileBuffer, unsigned int& size);

    char* buffer;
};

inline ModelImporter::~ModelImporter()
{
    delete[] buffer;
}

