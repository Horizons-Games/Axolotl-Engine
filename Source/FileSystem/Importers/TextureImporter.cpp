#include "TextureImporter.h"

#include "EngineLog.h"
#include "FileSystem/Data.h"

#include <GL/glew.h>
#include <DirectXTex/DirectXTex.h>

void TextureImporter::Start(const char* filePath, DataTexture* ourTexture)
{
	ENGINE_LOG("Import texture from %s", filePath);

	std::string narrowString(filePath);
	std::wstring wideString = std::wstring(narrowString.begin(), narrowString.end());
	const wchar_t* path = wideString.c_str();

	DirectX::TexMetadata md;
	DirectX::ScratchImage img, flippedImg, dcmprsdImg;

	HRESULT result = DirectX::LoadFromDDSFile(path, DirectX::DDS_FLAGS::DDS_FLAGS_NONE, &md, img);

	if (FAILED(result))
	{
		result = DirectX::LoadFromTGAFile(path, &md, img);

		if (FAILED(result))
		{
			result = DirectX::LoadFromWICFile(path, DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &md, img);

			result = DirectX::FlipRotate(img.GetImages(), img.GetImageCount(), img.GetMetadata(),
				DirectX::TEX_FR_FLAGS::TEX_FR_FLIP_VERTICAL, flippedImg);
		}
		else
		{
			result = DirectX::FlipRotate(img.GetImages(), img.GetImageCount(), img.GetMetadata(),
				DirectX::TEX_FR_FLAGS::TEX_FR_FLIP_VERTICAL, flippedImg);
		}
	}
	else
	{
		result = DirectX::Decompress(img.GetImages(), img.GetImageCount(),
			md, DXGI_FORMAT_UNKNOWN, dcmprsdImg);

		result = DirectX::FlipRotate(dcmprsdImg.GetImages(), dcmprsdImg.GetImageCount(),
			dcmprsdImg.GetMetadata(), DirectX::TEX_FR_FLAGS::TEX_FR_FLIP_VERTICAL, flippedImg);
	}

	result = DirectX::SaveToDDSFile(img.GetImages(), img.GetImageCount(), img.GetMetadata(), DirectX::DDS_FLAGS_NONE, path);

	GLint internalFormat;
	GLenum format, type;

	switch (img.GetMetadata().format)
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		internalFormat = GL_RGBA8;
		format = GL_RGBA;
		type = GL_UNSIGNED_BYTE;
		break;
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
		internalFormat = GL_RGBA8;
		format = GL_BGRA;
		type = GL_UNSIGNED_BYTE;
		break;
	case DXGI_FORMAT_B5G6R5_UNORM:
		internalFormat = GL_RGB8;
		format = GL_BGR;
		type = GL_UNSIGNED_BYTE;
		break;
	case DXGI_FORMAT_BC1_UNORM:
		internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		format = GL_RGBA;
		type = GL_UNSIGNED_BYTE;
		break;
	default:
		assert(false && "Unsupported format");
	}

	ourTexture->width = img.GetMetadata().width;
	ourTexture->height = img.GetMetadata().height;

	ourTexture->internalFormat = internalFormat;
	ourTexture->format = format;
	ourTexture->type = type;

	ourTexture->pixelsSize = img.GetPixelsSize();
	ourTexture->pixels = img.GetPixels();

	//Actualize metafile if needed
}

uint64_t TextureImporter::Save(const DataTexture* ourTexture, char*& fileBuffer)
{
	unsigned int header[4] = 
	{ 
		ourTexture->width,
		ourTexture->height,
		ourTexture->format,
		ourTexture->pixelsSize
	};

	unsigned int size = sizeof(header) + sizeof(unsigned char) * ourTexture->pixelsSize;

	char* cursor = new char[size];

	fileBuffer = cursor;

	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

	bytes = sizeof(unsigned char) * ourTexture->pixelsSize;
	memcpy(cursor, ourTexture->pixels, bytes);

	// Provisional return, here we have to return serialize UID for the object
	return 0;
}

void TextureImporter::Load(const char* fileBuffer, DataTexture* ourTexture)
{
	unsigned int header[4];
	memcpy(header, fileBuffer, sizeof(header));

	ourTexture->width = header[0];
	ourTexture->height = header[1];
	ourTexture->format = header[2];
	ourTexture->pixelsSize = header[3];

	fileBuffer += sizeof(header);

	ourTexture->pixels = new unsigned char[ourTexture->pixelsSize];

	memcpy(ourTexture->pixels, fileBuffer, sizeof(unsigned char) * ourTexture->pixelsSize);
}