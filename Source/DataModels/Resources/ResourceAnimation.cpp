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

void ResourceAnimation::InternalLoad()
{
	//TODO: Maybe AnimationImporter Import should be called here?
}

void ResourceAnimation::InternalUnload()
{
	for (Channel* channel : channels)
	{
		delete channel;
	}

	channels.clear();
}
