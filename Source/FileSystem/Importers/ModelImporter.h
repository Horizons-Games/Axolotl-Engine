#pragma once
#include "DataModels/Resources/ResourceModel.h"
#include "FileSystem/Importers/Importer.h"

class aiScene;
class aiMesh;
class aiString;

class ModelImporter : public Importer<ResourceModel>
{
public:
	ModelImporter();
	~ModelImporter() override;

	void Import(const char* filePath, std::shared_ptr<ResourceModel> resource) override;
	void Load(const char* fileBuffer, std::shared_ptr<ResourceModel> resource) override;

protected:
	void Save(const std::shared_ptr<ResourceModel>& resource, char*& fileBuffer, unsigned int& size) override;

private:
	void ImportMaterials(const aiScene* scene, const char* filePath, std::shared_ptr<ResourceModel>& resource);
	void ImportMeshes(const aiScene* scene, const char* filePath, std::shared_ptr<ResourceModel>& resource);

	void CheckPathMaterial(const char* filePath, const aiString& file, std::string& dataBuffer);

	void SaveInfoMesh(const aiMesh* ourMesh, char*& fileBuffer, unsigned int& size);
};
