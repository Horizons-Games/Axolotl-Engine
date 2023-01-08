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
	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
	unsigned int GetFormat() const;
	unsigned int GetInternalFormat() const;
	unsigned int GetImageType() const;
	unsigned char* GetPixels() const;
	unsigned int GetPixelsSize() const;

	void SetWidth(unsigned int width);
	void SetHeight(unsigned int height);
	void SetFormat(unsigned int format);
	void SetInternalFormat(unsigned int internalFormat);
	void SetImageType(unsigned int imageType);
	void SetPixels(unsigned char* pixels);
	void SetPixelsSize(unsigned int pixelsSize);

private:
	void CreateTexture();

	unsigned int glTexture = 0;
	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int format = 0;
	unsigned int internalFormat = 0;
	unsigned int imageType = 0;
	unsigned char* pixels = nullptr;
	unsigned int pixelsSize = 0;
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

inline unsigned int ResourceTexture::GetWidth() const
{
	return width;
}

inline unsigned int ResourceTexture::GetHeight() const
{
	return height;
}

inline unsigned int ResourceTexture::GetFormat() const
{
	return format;
}

inline unsigned int ResourceTexture::GetInternalFormat() const
{
	return internalFormat;
}

inline unsigned int ResourceTexture::GetImageType() const
{
	return imageType;
}

inline unsigned char* ResourceTexture::GetPixels() const
{
	return pixels;
}

inline unsigned int ResourceTexture::GetPixelsSize() const
{
	return pixelsSize;
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

inline void ResourceTexture::SetImageType(unsigned int imageType)
{
	this->imageType = imageType;
}

inline void ResourceTexture::SetPixels(unsigned char* pixels)
{
	this->pixels = pixels;
}

inline void ResourceTexture::SetPixelsSize(unsigned int pixelsSize)
{
	this->pixelsSize = pixelsSize;
}
