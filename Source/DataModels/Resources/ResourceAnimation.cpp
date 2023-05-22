#include "ResourceAnimation.h"

ResourceAnimation::ResourceAnimation(UID resourceId,
									 const std::string& fileName,
									 const std::string& assetsPath,
									 const std::string& libraryPath) :
	Resource(resourceId, fileName, assetsPath, libraryPath),
	duration(0.0f)
{
}

ResourceAnimation::~ResourceAnimation()
{
	for (auto it = channels.begin(); it != channels.end(); ++it)
	{
		delete it->second;
	}

	channels.clear();
}

void ResourceAnimation::InternalUnload()
{
}
