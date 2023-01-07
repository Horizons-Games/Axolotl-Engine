#include "TextureImporter.h"
#include "FileSystem/Data.h"
#include "EngineLog.h"

#include <DirectXTex/DirectXTex.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
	
	if (!FAILED(result)) {
		result = DirectX::LoadFromDDSFile(path, DirectX::DDS_FLAGS::DDS_FLAGS_NONE, &md, img);
	}

	//TODO Call Resource Start or Init function with all this info
	ourTexture->width = img.GetMetadata().width;
	ourTexture->height = img.GetMetadata().height;

	ourTexture->format = img.GetMetadata().format;
	ourTexture->dataSize = img.GetPixelsSize();
	ourTexture->imageData = img.GetPixels();
}

uint64_t TextureImporter::Save(const DataTexture* ourTexture, char*& fileBuffer)
{
	// Provisional return, here we have to return serialize UID for the object
	return 0;
}

void TextureImporter::Load(const char* fileBuffer, DataTexture* ourTexture)
{
	
}