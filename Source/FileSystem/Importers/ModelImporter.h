#pragma once
#include "DataModels/Resources/ResourceModel.h"
#include "FileSystem/Importers/Importer.h"

class aiScene;
class aiMesh;
class aiMaterial;
class aiNode;
class aiString;
class aiAnimation;

class ResourceMesh;
class ResourceMaterial;
class ResourceAnimation;

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
	void ImportNode(const aiScene* scene,
					const char* filePath,
					const std::shared_ptr<ResourceModel>& resource,
					const aiNode* parent,
					int parentIdx,
					const float4x4& accTransform);
	std::shared_ptr<ResourceMesh> ImportMesh(const aiMesh* mesh, const char* filePath, int iteration);
	std::shared_ptr<ResourceMaterial> ImportMaterial(const aiMaterial* material, const char* filePath, int iteration);
	void ImportAnimations(const aiScene* scene, const std::shared_ptr<ResourceModel>& resource);

	void CheckPathMaterial(const char* filePath, const aiString& file, std::string& dataBuffer);

	void SaveInfoMesh(const aiMesh* ourMesh, char*& fileBuffer, unsigned int& size);
	void SaveInfoAnimation(const aiAnimation* animation, char*& fileBuffer, unsigned int& size);
};
