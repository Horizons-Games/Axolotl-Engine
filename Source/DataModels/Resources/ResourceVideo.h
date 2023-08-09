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

	void SaveImporterOptions(Json& meta) override;
	void LoadImporterOptions(Json& meta) override;

	void SaveLoadOptions(Json& meta) override;
	void LoadLoadOptions(Json& meta) override;

protected:
	void InternalLoad() override;
	void InternalUnload() override;

private:
	unsigned int frameRate;
	unsigned int numFrames;
};

inline ResourceType ResourceVideo::GetType() const
{
	return ResourceType::Video;
}