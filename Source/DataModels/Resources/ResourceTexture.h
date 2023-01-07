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

	void Load() override;
	void Unload() override;

	unsigned int GetGlTexture() const;

	void SetWidth(unsigned int width);
	void SetHeight(unsigned int height);
	void SetFormat(unsigned int format);
	void SetInternalFormat(unsigned int internalFormat);
	void SetType(unsigned int type);
	void SetPixels(char* pixels);

private:
	void CreateTexture();

	unsigned int glTexture = 0;
	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int format = 0;
	unsigned int internalFormat = 0;
	unsigned int type = 0;
	char* pixels = nullptr;
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

inline unsigned int ResourceTexture::GetGlTexture() const
{
	return glTexture;
}

inline void ResourceTexture::SetWidth(unsigned int width)
{
	this->width = width;
}

inline void ResourceTexture::SetHeight(unsigned int height)
{
	this->height = height;
}

inline void ResourceTexture::SetFormat(unsigned int format)
{
	this->format = format;
}

inline void ResourceTexture::SetInternalFormat(unsigned int internalFormat)
{
	this->internalFormat = internalFormat;
}

inline void ResourceTexture::SetType(unsigned int type)
{
	this->type = type;
}

inline void ResourceTexture::SetPixels(char* pixels)
{
	this->pixels = pixels;
}
