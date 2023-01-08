#include "TextureImporter.h"

#include "EngineLog.h"

#include <GL/glew.h>
#include <DirectXTex/DirectXTex.h>

#define DDS_TEXTURE_EXTENSION ".dds"

void TextureImporter::Import(const char* filePath, std::shared_ptr<ResourceTexture> ourTexture)
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

	narrowString = ourTexture->GetLibraryPath() + DDS_TEXTURE_EXTENSION;
	wideString = std::wstring(narrowString.begin(), narrowString.end());
	path = wideString.c_str();

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

	ourTexture->SetWidth(img.GetMetadata().width);
	ourTexture->SetHeight(img.GetMetadata().height);

	ourTexture->SetInternalFormat(internalFormat);
	ourTexture->SetFormat(format);
	ourTexture->SetImageType(type);

	ourTexture->SetPixelsSize(img.GetPixelsSize());
	ourTexture->SetPixels(img.GetPixels());

	//Actualize metafile if needed
}

uint64_t TextureImporter::Save(std::shared_ptr<ResourceTexture> ourTexture, char*& fileBuffer)
{
	unsigned int header[4] = 
	{ 
		ourTexture->GetWidth(),
		ourTexture->GetHeight(),
		ourTexture->GetFormat(),
		ourTexture->GetPixelsSize()
	};

	unsigned int size = sizeof(header) + sizeof(unsigned char) * ourTexture->GetPixelsSize();

	char* cursor = new char[size];

	fileBuffer = cursor;

	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

	bytes = sizeof(unsigned char) * ourTexture->GetPixelsSize();
	memcpy(cursor, ourTexture->GetPixels(), bytes);

	// Provisional return, here we have to return serialize UID for the object
	return 0;
}

void TextureImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceTexture> ourTexture)
{
	unsigned int header[4];
	memcpy(header, fileBuffer, sizeof(header));

	ourTexture->SetWidth(header[0]);
	ourTexture->SetHeight(header[1]);
	ourTexture->SetFormat(header[2]);
	ourTexture->SetPixelsSize(header[3]);

	fileBuffer += sizeof(header);

	unsigned char* pixels = new unsigned char[ourTexture->GetPixelsSize()];
	memcpy(pixels, fileBuffer, sizeof(unsigned char) * ourTexture->GetPixelsSize());
	ourTexture->SetPixels(pixels);
} 