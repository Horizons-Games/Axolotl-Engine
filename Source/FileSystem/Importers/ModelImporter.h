#pragma once
#include "FileSystem/Importers/Importer.h"
#include "DataModels/Resources/ResourceModel.h"

class aiScene;
class aiMesh;
class aiString;

class ModelImporter : public Importer<ResourceModel>
{
public:
    ModelImporter() = default;
    ~ModelImporter();

    void Import(const char* filePath, std::shared_ptr<ResourceModel> resource) override;

protected:
    uint64_t Save(const std::shared_ptr<ResourceModel>& resource, char*& fileBuffer, unsigned int& size) override;
    void Load(const char* fileBuffer, std::shared_ptr<ResourceModel>& resource) override;

private:
    void ImportMaterials(const aiScene* scene, const char* filePath, std::shared_ptr<ResourceModel>& resource);
    void ImportMeshes(const aiScene* scene, const char* filePath, std::shared_ptr<ResourceModel>& resource);

    void CheckPathMaterial(const char* filePath, const aiString& file, std::string& dataBuffer);

    void SaveInfoMaterial(const std::vector<std::string>& pathTextures, char*& fileBuffer, unsigned int& size);
    void SaveInfoMesh(const aiMesh* ourMesh, char*& fileBuffer, unsigned int& size);

    char* buffer;
};

inline ModelImporter::~ModelImporter()
{
    delete[] buffer;
}

