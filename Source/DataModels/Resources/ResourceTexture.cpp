#include "ResourceTexture.h"

#include "GL/glew.h"
#include "FileSystem/Json.h"

ResourceTexture::ResourceTexture(UID resourceUID, const std::string& fileName, const std::string& assetsPath,
	const std::string& libraryPath) : Resource(resourceUID, fileName, assetsPath, libraryPath),
	glTexture(0), width(0), height(0), format(0), internalFormat(0), imageType(0), pixelsSize(0),
	options(std::make_shared<OptionsTexture>())
{
}

ResourceTexture::~ResourceTexture()
{
	this->Unload();
}

void ResourceTexture::InternalLoad()
{
	CreateTexture();
}

void ResourceTexture::InternalUnload()
{
	glDeleteTextures(1, &glTexture);
	glTexture = 0;
}

void ResourceTexture::SaveOptions(Json& meta)
{
	meta["min"] = (int) options->min;
	meta["mag"] = (int) options->mag;
	meta["wrapS"] = (int) options->wrapS;
	meta["wrapT"] = (int) options->wrapT;
	meta["mipMap"] = options->mipMap;
}

void ResourceTexture::LoadOptions(Json& meta)
{
	options->min = (TextureMinFilter)(int) meta["min"];
	options->mag = (TextureMagFilter)(int) meta["mag"];
	options->wrapS = (TextureWrap)(int) meta["wrapS"];
	options->wrapT = (TextureWrap)(int) meta["wrapT"];
	options->mipMap = meta["mipMap"];
}

void ResourceTexture::CreateTexture()
{
	glGenTextures(1, &glTexture);
	glBindTexture(GL_TEXTURE_2D, glTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, imageType, &(pixels[0]));

	glGenerateMipmap(GL_TEXTURE_2D);
}
