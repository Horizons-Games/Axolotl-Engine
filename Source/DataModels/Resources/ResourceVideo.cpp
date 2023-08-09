#include "StdAfx.h"
#include "ResourceVideo.h"

ResourceVideo::ResourceVideo(UID resourceUID,
										   const std::string& fileName,
										   const std::string& assetsPath,
										   const std::string& libraryPath):
	Resource(resourceUID, fileName, assetsPath, libraryPath),
	frameRate(0),
	numFrames(0)
{
}

ResourceVideo::~ResourceVideo()
{
}

void ResourceVideo::SaveImporterOptions(Json& meta)
{
}

void ResourceVideo::LoadImporterOptions(Json& meta)
{
}

void ResourceVideo::SaveLoadOptions(Json& meta)
{
}

void ResourceVideo::LoadLoadOptions(Json& meta)
{
}

void ResourceVideo::InternalLoad()
{
}

void ResourceVideo::InternalUnload()
{
}
