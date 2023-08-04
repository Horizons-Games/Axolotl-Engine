#pragma once

#include "Resource.h"
#include "ResourceTexture.h"

class ResourceVideo : virtual public Resource
{
public:
	ResourceVideo(UID resourceUID,
					const std::string& fileName,
					const std::string& assetsPath,
					const std::string& libraryPath);
	virtual ~ResourceVideo() override;
	ResourceType GetType() const override;

private:
	unsigned int frameRate;
	unsigned int numFrames;
};