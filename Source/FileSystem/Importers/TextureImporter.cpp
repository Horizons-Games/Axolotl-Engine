#include "TextureImporter.h"

#include "EngineLog.h"
#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"

#include <GL/glew.h>
#include <DirectXTex/DirectXTex.h>

#define DDS_TEXTURE_EXTENSION ".dds"

void TextureImporter::Import(const char* filePath, std::shared_ptr<ResourceTexture> resource)
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

	narrowString = resource->GetLibraryPath() + DDS_TEXTURE_EXTENSION;
	wideString = std::wstring(narrowString.begin(), narrowString.end());
	path = wideString.c_str();

	result = DirectX::SaveToDDSFile(flippedImg.GetImages(), flippedImg.GetImageCount(), flippedImg.GetMetadata(), DirectX::DDS_FLAGS_NONE, path);

	GLint internalFormat;
	GLenum format, type;

	switch (flippedImg.GetMetadata().format)
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

	resource->SetWidth(flippedImg.GetMetadata().width);
	resource->SetHeight(flippedImg.GetMetadata().height);

	resource->SetInternalFormat(internalFormat);
	resource->SetFormat(format);
	resource->SetImageType(type);

	resource->SetPixelsSize(flippedImg.GetPixelsSize());

	std::vector<uint8_t> pixels(flippedImg.GetPixels(),flippedImg.GetPixels() + flippedImg.GetPixelsSize());

	resource->SetPixels(pixels);

	char* buffer{};
	unsigned int size;
	Save(resource, buffer, size);
	App->fileSystem->Save((resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), buffer, size);

	delete buffer;
}

void TextureImporter::Save(const std::shared_ptr<ResourceTexture>& resource, char*& fileBuffer, unsigned int& size)
{
	unsigned int header[6] =
	{
		resource->GetWidth(),
		resource->GetHeight(),
		resource->GetFormat(),
		resource->GetInternalFormat(),
		resource->GetImageType(),
		resource->GetPixelsSize()
	};

	size = sizeof(header) + sizeof(unsigned char) * resource->GetPixelsSize();

	char* cursor = new char[size];

	fileBuffer = cursor;

	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

	bytes = sizeof(unsigned char) * resource->GetPixelsSize();
	memcpy(cursor, &(resource->GetPixels()[0]), bytes);
}

void TextureImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceTexture> resource)
{
	unsigned int header[6];
	memcpy(header, fileBuffer, sizeof(header));

	resource->SetWidth(header[0]);
	resource->SetHeight(header[1]);
	resource->SetFormat(header[2]);
	resource->SetInternalFormat(header[3]);
	resource->SetImageType(header[4]);
	resource->SetPixelsSize(header[5]);

	fileBuffer += sizeof(header);

	unsigned char* pixelsPointer = new unsigned char[resource->GetPixelsSize()];
	memcpy(pixelsPointer, fileBuffer, sizeof(unsigned char) * resource->GetPixelsSize());
	std::vector<unsigned char> pixels(pixelsPointer, pixelsPointer + resource->GetPixelsSize());
	resource->SetPixels(pixels);

	delete[] pixelsPointer;
} 