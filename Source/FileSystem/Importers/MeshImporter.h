#pragma once
#include "DataModels/Resources/ResourceMesh.h"
#include "FileSystem/Importers/Importer.h"

class MeshImporter : public Importer<ResourceMesh>
{
public:
	MeshImporter();
	~MeshImporter() override;

	void Import(const char* filePath, std::shared_ptr<ResourceMesh> resource) override;
	void Load(const char* fileBuffer, std::shared_ptr<ResourceMesh> resource) override;

protected:
	void Save(const std::shared_ptr<ResourceMesh>& resource, char*& fileBuffer, unsigned int& size) override;
};
