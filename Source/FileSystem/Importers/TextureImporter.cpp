#include "TextureImporter.h"

#include "Application.h"
#include "FileSystem/Json.h"
#include "FileSystem/ModuleFileSystem.h"
#include "Resources/ResourceTexture.h"

#include <DirectXTex/DirectXTex.h>
#include <GL/glew.h>

#define DDS_TEXTURE_EXTENSION ".dds"

TextureImporter::TextureImporter()
{
}

TextureImporter::~TextureImporter()
{
}

void TextureImporter::Import(const char* filePath, std::shared_ptr<ResourceTexture> resource)
{
	bool cubeMap = false;
	ENGINE_LOG("Import texture from %s", filePath);

	ImportOptionsTexture options = resource->GetImportOptions();

	std::string narrowString(filePath);
	std::wstring wideString = std::wstring(narrowString.begin(), narrowString.end());
	const wchar_t* path = wideString.c_str();

	DirectX::TexMetadata md;
	DirectX::ScratchImage* imgResult, compressImg;
	DirectX::ScratchImage img, flippedImg, dcmprsdImg;

	HRESULT result = DirectX::LoadFromDDSFile(path, DirectX::DDS_FLAGS::DDS_FLAGS_NONE, &md, img);

	if (FAILED(result)) // DDS failed try with TGA
	{
		result = DirectX::LoadFromTGAFile(path, &md, img);

		if (FAILED(result)) // TGA failed try with WIC
		{
			result = DirectX::LoadFromWICFile(path, DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &md, img);
			if (FAILED(result)) // WIC failed try with HDR
			{
				result = DirectX::LoadFromHDRFile(path, &md, img);
				cubeMap = true;
				if (FAILED(result))
				{
					
					ENGINE_LOG("Cannot load the texture.");
				}
			}
		}
	}
	else // DDS success
	{
		result = DirectX::Decompress(img.GetImages(), img.GetImageCount(), md, DXGI_FORMAT_UNKNOWN, dcmprsdImg);

		img = std::move(dcmprsdImg);
	}

	imgResult = &img;
	if (options.flipVertical && options.flipHorizontal)
	{
		result = DirectX::FlipRotate(img.GetImages(),
									 img.GetImageCount(),
									 img.GetMetadata(),
									 DirectX::TEX_FR_FLAGS::TEX_FR_ROTATE180,
									 flippedImg);

		if (!FAILED(result))
		{
			imgResult = &flippedImg;
		}
	}
	else if (options.flipVertical)
	{
		result = DirectX::FlipRotate(img.GetImages(),
									 img.GetImageCount(),
									 img.GetMetadata(),
									 DirectX::TEX_FR_FLAGS::TEX_FR_FLIP_VERTICAL,
									 flippedImg);

		if (!FAILED(result))
		{
			imgResult = &flippedImg;
		}
	}
	else if (options.flipHorizontal)
	{
		result = DirectX::FlipRotate(img.GetImages(),
									 img.GetImageCount(),
									 img.GetMetadata(),
									 DirectX::TEX_FR_FLAGS::TEX_FR_FLIP_HORIZONTAL,
									 flippedImg);

		if (!FAILED(result))
		{
			imgResult = &flippedImg;
		}
	}

	DXGI_FORMAT compressFormat;
	switch (options.compression)
	{
		case 0:
			compressFormat = DXGI_FORMAT_BC1_UNORM;
			break;
		case 1:
			compressFormat = DXGI_FORMAT_BC3_UNORM;
			break;
		case 2:
			compressFormat = DXGI_FORMAT_BC4_UNORM;
			break;
		case 3:
			compressFormat = DXGI_FORMAT_BC5_UNORM;
			break;
		case 4:
			compressFormat = DXGI_FORMAT_BC6H_SF16;
			break;
		case 5:
			compressFormat = DXGI_FORMAT_BC7_UNORM;
			break;
		default:
			break;
	}

	
	if (options.compression > -1)
	{
		result = DirectX::Compress(imgResult->GetImages(),
								   imgResult->GetImageCount(),
								   imgResult->GetMetadata(),
								   compressFormat,
								   DirectX::TEX_COMPRESS_DEFAULT,
								   DirectX::TEX_THRESHOLD_DEFAULT,
								   compressImg);

		if (FAILED(result)) // Compression fails
		{
		}
	}
	else
	{
		HRESULT result = compressImg.InitializeFromImage(
			*imgResult->GetImages(), false, DirectX::CP_FLAGS_NONE);
		if (FAILED(result))
		{
			// Handle the error appropriately
		}
	}
	GLint internalFormat;
	GLenum format, type;

	switch (compressImg.GetMetadata().format)
	{
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			internalFormat = GL_RGBA16F;
			format = GL_RGBA;
			type = GL_FLOAT;
			break;
		case DXGI_FORMAT_R8_UNORM:
			internalFormat = GL_R8;
			format = GL_RED;
			type = GL_BYTE;
			break;
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
		case DXGI_FORMAT_BC2_UNORM:
			internalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			break;
		case DXGI_FORMAT_BC3_UNORM:
			internalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			break;
		case DXGI_FORMAT_BC4_UNORM:
			internalFormat = GL_COMPRESSED_RED_RGTC1;
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			break;
		case DXGI_FORMAT_BC5_UNORM:
			internalFormat = GL_COMPRESSED_RG_RGTC2;
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			break;
		case DXGI_FORMAT_BC6H_SF16:
			internalFormat = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			break;
		case DXGI_FORMAT_BC7_UNORM:
			internalFormat = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			break;
		default:
			assert(false && "Unsupported format");
	}

	resource->SetWidth((unsigned int) compressImg.GetMetadata().width);
	resource->SetHeight((unsigned int) compressImg.GetMetadata().height);

	resource->SetInternalFormat(internalFormat);
	resource->SetFormat(format);
	resource->SetImageType(type);

	resource->SetPixelsSize((unsigned int) compressImg.GetPixelsSize());

	std::vector<uint8_t> pixels(compressImg.GetPixels(), compressImg.GetPixels() + compressImg.GetPixelsSize());

	resource->SetPixels(pixels);


	// Save in DDS format
	narrowString = resource->GetLibraryPath() + DDS_TEXTURE_EXTENSION;
	wideString = std::wstring(narrowString.begin(), narrowString.end());
	path = wideString.c_str();

	result = DirectX::SaveToDDSFile(
		compressImg.GetImages(), compressImg.GetImageCount(), compressImg.GetMetadata(), DirectX::DDS_FLAGS_NONE, path);

	//Save(resource, buffer, size);

	/* if (!cubeMap)
	{
		
	}
	else
	{
		char* buffer{};
		unsigned int size;
		Save(resource, buffer, size);
		App->GetModule<ModuleFileSystem>()->Save(
			(resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), buffer, size);

		delete buffer;
	}*/
}

void TextureImporter::Save(const std::shared_ptr<ResourceTexture>& resource, char*& fileBuffer, unsigned int& size)
{
	unsigned int header[6] = { resource->GetWidth(),		  resource->GetHeight(),	resource->GetFormat(),
							   resource->GetInternalFormat(), resource->GetImageType(), resource->GetPixelsSize() };

	unsigned int options[5] = { static_cast<unsigned int>(resource->GetLoadOptions().min),
								static_cast<unsigned int>(resource->GetLoadOptions().mag),
								static_cast<unsigned int>(resource->GetLoadOptions().wrapS),
								static_cast<unsigned int>(resource->GetLoadOptions().wrapT),
								resource->GetLoadOptions().mipMap };

	size = sizeof(header) + sizeof(unsigned char) * resource->GetPixelsSize() + sizeof(options);

	char* cursor = new char[size];

	fileBuffer = cursor;

	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

	bytes = sizeof(unsigned char) * resource->GetPixelsSize();
	memcpy(cursor, &(resource->GetPixels()[0]), bytes);

	cursor += bytes;

	bytes = sizeof(options);
	memcpy(cursor, options, bytes);
}

void TextureImporter::Load(const char* filePath, std::shared_ptr<ResourceTexture> resource)
{
	std::string narrowString(filePath);
	std::wstring wideString = std::wstring(narrowString.begin(), narrowString.end());
	const wchar_t* path = wideString.c_str();


	DirectX::TexMetadata md;
	DirectX::ScratchImage* imgResult;
	DirectX::ScratchImage img;

	
	HRESULT result = DirectX::LoadFromDDSFile(path, DirectX::DDS_FLAGS::DDS_FLAGS_NONE, &md, img);

	if (!FAILED(result)) // DDS load
	{
		imgResult = &img;

		resource->SetWidth((unsigned int) imgResult->GetMetadata().width);
		resource->SetHeight((unsigned int) imgResult->GetMetadata().height);

		GLint internalFormat;
		GLenum format, type;

		switch (imgResult->GetMetadata().format)
		{
			case DXGI_FORMAT_R32G32B32A32_FLOAT:
				internalFormat = GL_RGBA16F;
				format = GL_RGBA;
				type = GL_FLOAT;
				break;
			case DXGI_FORMAT_R8_UNORM:
				internalFormat = GL_R8;
				format = GL_RED;
				type = GL_BYTE;
				break;
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
			case DXGI_FORMAT_BC3_UNORM:
				internalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
				format = GL_RGBA;
				type = GL_UNSIGNED_BYTE;
				break;
			case DXGI_FORMAT_BC4_UNORM:
				internalFormat = GL_COMPRESSED_RED_RGTC1;
				format = GL_RGBA;
				type = GL_UNSIGNED_BYTE;
				break;
			case DXGI_FORMAT_BC5_UNORM:
				internalFormat = GL_COMPRESSED_RG_RGTC2;
				format = GL_RGBA;
				type = GL_UNSIGNED_BYTE;
				break;
			case DXGI_FORMAT_BC6H_SF16:
				internalFormat = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
				format = GL_RGBA;
				type = GL_UNSIGNED_BYTE;
				break;
			case DXGI_FORMAT_BC7_UNORM:
				internalFormat = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;
				format = GL_RGBA;
				type = GL_UNSIGNED_BYTE;
				break;
			default:
				assert(false && "Unsupported format");
		}

		resource->SetInternalFormat(internalFormat);
		resource->SetFormat(format);
		resource->SetImageType(type);

		resource->SetPixelsSize((unsigned int) imgResult->GetPixelsSize());

		uint8_t* pixels = imgResult->GetPixels();

		std::vector<uint8_t> vpixels(pixels, pixels + imgResult->GetPixelsSize());

		resource->SetPixels(vpixels);
	}
	else //Cubemap
	{
		char* fileBuffer = {};
		ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();
		fileSystem->Load(filePath, fileBuffer);
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

		fileBuffer += sizeof(unsigned char) * resource->GetPixelsSize();

		delete[] pixelsPointer;
	}
	

#ifndef ENGINE
	unsigned int options[5];
	memcpy(options, fileBuffer, sizeof(options));

	resource->GetLoadOptions().min = (TextureMinFilter) options[0];
	resource->GetLoadOptions().mag = (TextureMagFilter) options[1];
	resource->GetLoadOptions().wrapS = (TextureWrap) options[2];
	resource->GetLoadOptions().wrapT = (TextureWrap) options[3];
	resource->GetLoadOptions().mipMap = options[4];
#endif // ENGINE
}