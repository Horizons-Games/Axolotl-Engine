#pragma once

#include "Resource.h"
#include "ResourceTexture.h"

class ResourceTextureVideo : virtual public Resource
{
public:
	ResourceTextureVideo(UID resourceUID,
					const std::string& fileName,
					const std::string& assetsPath,
					const std::string& libraryPath);
	virtual ~ResourceTextureVideo() override;
	ResourceType GetType() const override;

private:
	unsigned int frameRate;
	unsigned int numFrames;
	std::vector<std::shared_ptr<ResourceTexture>> frames;
};