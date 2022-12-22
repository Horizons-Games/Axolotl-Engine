#include "ModuleTexture.h"
#include "Globals.h"

#include <sys/stat.h>

#include <GL/glew.h>
#include <DirectXTex/DirectXTex.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

ModuleTexture::ModuleTexture()
{
}

ModuleTexture::~ModuleTexture()
{
}

bool ModuleTexture::Init()
{
	return true;
}

bool ModuleTexture::Start()
{
	return true;
}

update_status ModuleTexture::Update()
{
	return UPDATE_CONTINUE;
}

GLuint ModuleTexture::Load(const char* fileName, const char* filePath, 
unsigned int & textureWidth, unsigned int & textureHeight)
{
	ENGINE_LOG("---- Loading texture ----");

	GLuint texture;

	std::string texturePath = TEXTURES_PATH + std::string(fileName);
	
	struct stat buffer;
	// Cheking by name
	if (stat(fileName, &buffer) != 0)
	{
		//Checking in the fbx folder
		if (stat((std::string(filePath) + std::string(fileName)).c_str(), &buffer) != 0)
		{
			// Cheking in textures folder
			if (stat((TEXTURES_PATH + std::string(fileName)).c_str(), &buffer) != 0)
			{
				ENGINE_LOG("Texture not found!");
			}
			else
				texturePath = TEXTURES_PATH + std::string(fileName);
		}
		else
			texturePath = filePath + std::string(fileName);
	}
	else
		texturePath = std::string(fileName);

	ENGINE_LOG("Loading texture %s", &texturePath[0]);

	std::string narrowString(texturePath);
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

	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, flippedImg.GetMetadata().width,
		flippedImg.GetMetadata().height, 0, format, type, flippedImg.GetPixels());

	glGenerateMipmap(GL_TEXTURE_2D);

	textureWidth = img.GetMetadata().width;
	textureHeight = img.GetMetadata().height;

	ENGINE_LOG("Texture %i loaded", texture);

	return texture;
}
