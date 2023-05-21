#include "ParticleSystemImporter.h"
#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"

ParticleSystemImporter::ParticleSystemImporter()
{
}

ParticleSystemImporter::~ParticleSystemImporter()
{
}

void ParticleSystemImporter::Import
(const char* filePath, std::shared_ptr<ResourceParticleSystem> resource)
{
	
}

void ParticleSystemImporter::Save
(const std::shared_ptr<ResourceParticleSystem>& resource,
	char*& fileBuffer, unsigned int& size)
{

}

void ParticleSystemImporter::Load
(const char* fileBuffer, std::shared_ptr<ResourceParticleSystem> resource)
{
	
}
