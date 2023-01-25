#include "ResourceTexture.h"

#include "GL/glew.h"
#include "FileSystem/Json.h"

//enum class TextureCompression {
//	NONE,
//	DXT1,
//	DXT3,
//	DXT5,
//	BC7
//};

struct LoadOptionsTexture
{
	GLenum min;
	GLenum mag;
	GLenum wrapS;
	GLenum wrapT;
	bool mipMap;

	LoadOptionsTexture() :
		min(GL_LINEAR_MIPMAP_LINEAR),
		mag(GL_LINEAR),
		wrapS(GL_REPEAT),
		wrapT(GL_REPEAT),
		mipMap(true)
	{}
};

ResourceTexture::ResourceTexture(UID resourceUID,
	const std::string& fileName,
	const std::string& assetsPath,
	const std::string& libraryPath) :
	Resource(resourceUID, fileName, assetsPath, libraryPath)
{
	importOptions = std::make_shared<ImportOptionsTexture>();
	loadOptions = std::make_shared<LoadOptionsTexture>();
}

void ResourceTexture::InternalLoad()
{
	this->CreateTexture();
}

void ResourceTexture::InternalUnload()
{
	glDeleteTextures(1, &glTexture);
	glTexture = 0;
}

void ResourceTexture::SaveOptions(Json& meta)
{
	meta["flip"] = importOptions->flip;

	meta["min"] = (int) loadOptions->min;
	meta["mag"] = (int) loadOptions->mag;
	meta["wrapS"] = (int) loadOptions->wrapS;
	meta["wrapT"] = (int) loadOptions->wrapT;
	meta["mipMap"] = loadOptions->mipMap;
}

void ResourceTexture::LoadOptions(Json& meta)
{
	importOptions->flip = meta["flip"];

	loadOptions->min = (int) meta["min"];
	loadOptions->mag = (int) meta["mag"];
	loadOptions->wrapS = (int) meta["wrapS"];
	loadOptions->wrapT = (int) meta["wrapT"];
	loadOptions->mipMap = meta["mipMap"];
}

void ResourceTexture::CreateTexture()
{
	glGenTextures(1, &glTexture);
	glBindTexture(GL_TEXTURE_2D, glTexture);

	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, loadOptions->mag);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, loadOptions->min);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, imageType, &(pixels[0]));

	glGenerateMipmap(GL_TEXTURE_2D);
}
