#include "ResourceTexture.h"

#include "FileSystem/Json.h"
#include "GL/glew.h"

ResourceTexture::ResourceTexture(UID resourceUID,
								 const std::string& fileName,
								 const std::string& assetsPath,
								 const std::string& libraryPath) :
	Resource(resourceUID, fileName, assetsPath, libraryPath),
	glTexture(0),
	width(0),
	height(0),
	format(0),
	internalFormat(0),
	imageType(0),
	pixelsSize(0),
	handle(0)
{
}

ResourceTexture::~ResourceTexture()
{
	glMakeTextureHandleNonResidentARB(handle);
	Unload();
}

void ResourceTexture::InternalUnload()
{
	glDeleteTextures(1, &glTexture);
	glTexture = 0;
}

void ResourceTexture::SaveImporterOptions(Json& meta)
{
	meta["flipVertical"] = importOptions.flipVertical;
	meta["flipHorizontal"] = importOptions.flipHorizontal;
	meta["compression"] = importOptions.compression;
}

void ResourceTexture::LoadImporterOptions(Json& meta)
{
	importOptions.flipVertical = meta["flipVertical"];
	importOptions.flipHorizontal = meta["flipHorizontal"];
	importOptions.compression = meta["compression"];
}

void ResourceTexture::SaveLoadOptions(Json& meta)
{
	meta["min"] = (int) loadOptions.min;
	meta["mag"] = (int) loadOptions.mag;
	meta["wrapS"] = (int) loadOptions.wrapS;
	meta["wrapT"] = (int) loadOptions.wrapT;
	meta["mipMap"] = loadOptions.mipMap;
}

void ResourceTexture::LoadLoadOptions(Json& meta)
{
	loadOptions.min = (TextureMinFilter) (int) meta["min"];
	loadOptions.mag = (TextureMagFilter) (int) meta["mag"];
	loadOptions.wrapS = (TextureWrap) (int) meta["wrapS"];
	loadOptions.wrapT = (TextureWrap) (int) meta["wrapT"];
	loadOptions.mipMap = (bool) meta["mipMap"];
}

void ResourceTexture::CreateTexture()
{
	glGenTextures(1, &glTexture);
	glBindTexture(GL_TEXTURE_2D, glTexture);

	int compressFormat = -1;
	int byteSize = 0;
	
	switch (importOptions.compression)
	{
		case 0:

			compressFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
			byteSize = 8;
			break;
		case 1:
			compressFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
			byteSize = 16;
			break;
		case 2:
			compressFormat = GL_COMPRESSED_RED_RGTC1;
			byteSize = 8;
			break;
		case 3:
			compressFormat = GL_COMPRESSED_RG_RGTC2;
			byteSize = 16;
			break;
		case 4:
			compressFormat = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
			byteSize = 16;
			break;
		case 5:
			compressFormat = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;
			byteSize = 16;
			break;
		default:
			break;
	}
	if (importOptions.compression == -1) glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, imageType, &(pixels[0]));
	else glCompressedTexImage2D(GL_TEXTURE_2D, 0, compressFormat, width, height, 0, ((width + 3) / 4) * ((height + 3) / 4) * byteSize, &(pixels[0]));
	if (loadOptions.mipMap)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetMagFilterEquivalence(loadOptions.mag));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetMinFilterEquivalence(loadOptions.min));

	glTexParameteri(GL_TEXTURE_2D,
					GL_TEXTURE_WRAP_S,
					format == GL_RGBA || format == GL_RGB ? GL_CLAMP_TO_EDGE
														  : GetWrapFilterEquivalence(loadOptions.wrapS));
	glTexParameteri(GL_TEXTURE_2D,
					GL_TEXTURE_WRAP_T,
					format == GL_RGBA || format == GL_RGB ? GL_CLAMP_TO_EDGE
														  : GetWrapFilterEquivalence(loadOptions.wrapT));
}

int ResourceTexture::GetMagFilterEquivalence(TextureMagFilter filter)
{
	switch (filter)
	{
		case TextureMagFilter::NEAREST:
			return GL_NEAREST;
		case TextureMagFilter::LINEAR:
			return GL_LINEAR;
		default:
			return GL_NEAREST;
	}
}

int ResourceTexture::GetMinFilterEquivalence(TextureMinFilter filter)
{
	switch (filter)
	{
		case TextureMinFilter::NEAREST:
			return GL_NEAREST;
		case TextureMinFilter::LINEAR:
			return GL_LINEAR;
		case TextureMinFilter::NEAREST_MIPMAP_NEAREST:
			return GL_NEAREST_MIPMAP_NEAREST;
		case TextureMinFilter::LINEAR_MIPMAP_NEAREST:
			return GL_LINEAR_MIPMAP_NEAREST;
		case TextureMinFilter::NEAREST_MIPMAP_LINEAR:
			return GL_NEAREST_MIPMAP_LINEAR;
		case TextureMinFilter::LINEAR_MIPMAP_LINEAR:
			return GL_LINEAR_MIPMAP_LINEAR;
		default:
			return GL_NEAREST;
	}
}

int ResourceTexture::GetWrapFilterEquivalence(TextureWrap filter)
{
	switch (filter)
	{
		case TextureWrap::REPEAT:
			return GL_REPEAT;
		case TextureWrap::CLAMP_TO_EDGE:
			return GL_CLAMP_TO_EDGE;
		case TextureWrap::CLAMP_TO_BORDER:
			return GL_CLAMP_TO_BORDER;
		case TextureWrap::MIRROR_REPEAT:
			return GL_MIRRORED_REPEAT;
		case TextureWrap::MIRROR_CLAMP_TO_EDGE:
			return GL_MIRROR_CLAMP_TO_EDGE;
		default:
			return GL_REPEAT;
	}
}

const uint64_t& ResourceTexture::GetHandle()
{
	if (!IsLoaded())
	{
		Load();
	}

	if (handle == 0)
	{
		handle = glGetTextureHandleARB(glTexture);
		glMakeTextureHandleResidentARB(handle);
	}
	return handle;
}