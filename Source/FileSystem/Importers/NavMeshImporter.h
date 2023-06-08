#pragma once
#include "DataModels/Resources/ResourceNavMesh.h"
#include "FileSystem/Importers/Importer.h"

class NavMeshImporter : public Importer<ResourceNavMesh>
{
public:
	NavMeshImporter();
	~NavMeshImporter() override;

	void Import(const char* filePath, std::shared_ptr<ResourceNavMesh> resource) override;
	void Load(const char* fileBuffer, std::shared_ptr<ResourceNavMesh> resource) override;

protected:
	void Save(const std::shared_ptr<ResourceNavMesh>& resource, char*& fileBuffer, unsigned int& size) override;
};
