#include "ResourceAnimation.h"

ResourceAnimation::ResourceAnimation(UID resourceId, const std::string& fileName, const std::string& assetsPath, 
	const std::string& libraryPath) : Resource(resourceId, fileName, assetsPath, libraryPath),
	channels(NULL), duration(0.0f)
{
}

ResourceAnimation::~ResourceAnimation()
{
	Unload();
}

void ResourceAnimation::InternalUnload()
{
	for (auto it = channels.begin(); it != channels.end(); ++it)
	{
		delete it->second;
	}

	channels.clear();
}
