#include "ResourceParticle.h"

ResourceParticle::ResourceParticle(UID resourceUID, const std::string& fileName, const std::string& assetsPath, 
	const std::string& libraryPath) :
	Resource(resourceUID, fileName, assetsPath, libraryPath)
{
}

ResourceParticle::~ResourceParticle()
{
	emitters.clear();
}
