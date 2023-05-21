#include "ResourceParticleSystem.h"

ResourceParticleSystem::ResourceParticleSystem(UID resourceUID, const std::string& fileName, 
	const std::string& assetsPath, const std::string& libraryPath) : 
	Resource(resourceUID, fileName, assetsPath, libraryPath)
{
}

ResourceParticleSystem::~ResourceParticleSystem()
{
}
