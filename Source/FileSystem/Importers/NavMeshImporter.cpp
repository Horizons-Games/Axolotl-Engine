#include "NavMeshImporter.h"

#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "Resources/ResourceNavMesh.h"

NavMeshImporter::NavMeshImporter()
{
}

NavMeshImporter::~NavMeshImporter()
{
}

void NavMeshImporter::Import(const char* filePath, std::shared_ptr<NavMeshImporter> resource)
{
}

void NavMeshImporter::Save(const std::shared_ptr<NavMeshImporter>& resource, char*& fileBuffer, unsigned int& size)
{
}

void NavMeshImporter::Load(const char* fileBuffer, std::shared_ptr<NavMeshImporter> resource)
{
}