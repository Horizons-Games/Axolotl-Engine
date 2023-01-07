#include "TextureImporter.h"

#include "EngineLog.h"
#include "FileSystem/Data.h"

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
	
	if (!FAILED(result)) 
	{
		result = DirectX::LoadFromDDSFile(path, DirectX::DDS_FLAGS::DDS_FLAGS_NONE, &md, img);
	}

	//TODO Call Resource Start or Init function with all this info
	ourTexture->width = img.GetMetadata().width;
	ourTexture->height = img.GetMetadata().height;

	ourTexture->format = img.GetMetadata().format;
	ourTexture->dataSize = img.GetPixelsSize();
	ourTexture->imageData = img.GetPixels();

	//Actualize metafile if needed
}

uint64_t TextureImporter::Save(const DataTexture* ourTexture, char*& fileBuffer)
{
	unsigned int header[4] = 
	{ 
		ourTexture->width,
		ourTexture->height,
		ourTexture->format,
		ourTexture->dataSize
	};

	unsigned int size = sizeof(header) + sizeof(unsigned char) * ourTexture->dataSize;

	char* cursor = new char[size];

	fileBuffer = cursor;

	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

	bytes = sizeof(unsigned char) * ourTexture->dataSize;
	memcpy(cursor, ourTexture->imageData, bytes);

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
	ourTexture->dataSize = header[3];

	fileBuffer += sizeof(header);

	ourTexture->imageData = new unsigned char[ourTexture->dataSize];

	memcpy(ourTexture->imageData, fileBuffer, sizeof(unsigned char) * ourTexture->dataSize);
}