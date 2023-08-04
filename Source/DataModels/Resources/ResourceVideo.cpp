#include "StdAfx.h"
#include "ResourceVideo.h"


ResourceVideo::ResourceVideo(UID resourceUID,
										   const std::string& fileName,
										   const std::string& assetsPath,
										   const std::string& libraryPath)
{
}

ResourceVideo::~ResourceVideo()
{
}

ResourceType ResourceVideo::GetType() const
{
	return ResourceType();
}
