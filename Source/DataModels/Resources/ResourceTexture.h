#pragma once

#include "Resource.h"

enum class TextureFormat
{
	Unknown,
	RGB,
	RGBA,
	BGR,
	BGRA,
	Luminance
};

class ResourceTexture : public Resource
{
public:
	ResourceTexture(UID resourceUID, const std::string& assetsPath, const std::string& libraryPath);
	~ResourceTexture() override = default;

	ResourceType GetType() const override;
	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
	TextureFormat GetFormat() const;

private:
	unsigned int width = 0;
	unsigned int height = 0;
	TextureFormat format = TextureFormat::Unknown;
};

inline ResourceTexture::ResourceTexture(UID resourceUID,
										const std::string& assetsPath,
										const std::string& libraryPath) :
	Resource(resourceUID, assetsPath, libraryPath)
{
}

inline ResourceType ResourceTexture::GetType() const
{
	return ResourceType::Texture;
}

inline unsigned int ResourceTexture::GetWidth() const
{
	return width;
}

inline unsigned int ResourceTexture::GetHeight() const
{
	return height;
}

inline TextureFormat ResourceTexture::GetFormat() const
{
	return format;
}
