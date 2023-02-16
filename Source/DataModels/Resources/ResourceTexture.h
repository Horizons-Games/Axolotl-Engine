#pragma once

#include "Resource.h"
#include <memory>

enum class TextureCompression {
	NONE,
	DXT1,
	DXT3,
	DXT5,
	BC7
};

enum class TextureMinFilter {
	NEAREST,
	LINEAR,
	NEAREST_MIPMAP_NEAREST,
	LINEAR_MIPMAP_NEAREST,
	NEAREST_MIPMAP_LINEAR,
	LINEAR_MIPMAP_LINEAR
};

enum class TextureMagFilter {
	NEAREST,
	LINEAR
};

enum class TextureWrap {
	REPEAT,
	CLAMP_TO_EDGE,
	CLAMP_TO_BORDER,
	MIRROR_REPEAT,
	MIRROR_CLAMP_TO_EDGE
};


struct OptionsTexture
{
	TextureMinFilter min;
	TextureMagFilter mag;
	TextureWrap wrapS;
	TextureWrap wrapT;
	bool mipMap;

	OptionsTexture() :
		min(TextureMinFilter::LINEAR_MIPMAP_LINEAR),
		mag(TextureMagFilter::LINEAR),
		wrapS(TextureWrap::REPEAT),
		wrapT(TextureWrap::REPEAT),
		mipMap(true)
	{}
};

class ResourceTexture : public Resource
{
public:
	ResourceTexture(UID resourceUID, const std::string& fileName, const std::string& assetsPath, const std::string& libraryPath);
	~ResourceTexture() override;

	ResourceType GetType() const override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	unsigned int GetGlTexture() const;
	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
	unsigned int GetFormat() const;
	unsigned int GetInternalFormat() const;
	unsigned int GetImageType() const;
	const std::vector<uint8_t>& GetPixels() const;
	unsigned int GetPixelsSize() const;

	std::shared_ptr<OptionsTexture>& GetOptions();

	void SetWidth(unsigned int width);
	void SetHeight(unsigned int height);
	void SetFormat(unsigned int format);
	void SetInternalFormat(unsigned int internalFormat);
	void SetImageType(unsigned int imageType);
	void SetPixels(std::vector<uint8_t>& pixels);
	void SetPixelsSize(unsigned int pixelsSize);

protected:
	void InternalLoad() override;
	void InternalUnload() override;
private:
	void CreateTexture();

	unsigned int glTexture;
	unsigned int width;
	unsigned int height;
	unsigned int format;
	unsigned int internalFormat;
	unsigned int imageType;
	std::vector<uint8_t> pixels;
	unsigned int pixelsSize;

	std::shared_ptr<OptionsTexture> options;
};

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

inline const std::vector<uint8_t>& ResourceTexture::GetPixels() const
{
	return pixels;
}

inline unsigned int ResourceTexture::GetPixelsSize() const
{
	return pixelsSize;
}

inline std::shared_ptr<OptionsTexture>& ResourceTexture::GetOptions()
{
	return this->options;
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

inline void ResourceTexture::SetPixels(std::vector<uint8_t>& pixels)
{
	this->pixels = pixels;
}

inline void ResourceTexture::SetPixelsSize(unsigned int pixelsSize)
{
	this->pixelsSize = pixelsSize;
}
