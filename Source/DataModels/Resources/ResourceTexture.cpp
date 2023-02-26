#include "ResourceTexture.h"

#include "GL/glew.h"
#include "FileSystem/Json.h"

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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, imageType, &(pixels[0]));

	glGenerateMipmap(GL_TEXTURE_2D);
}

void ResourceTexture::SetMinFilter(TextureMinFilter filter)
{
	for (ResourceTexture& texture : textures)
	{
		glBindTexture(GL_TEXTURE_2D, texture.gl_texture);
		switch (filter)
		{
		case TextureMinFilter::NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			break;
		case TextureMinFilter::LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			break;
		case TextureMinFilter::NEAREST_MIPMAP_NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			break;
		case TextureMinFilter::LINEAR_MIPMAP_NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			break;
		case TextureMinFilter::NEAREST_MIPMAP_LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			break;
		case TextureMinFilter::LINEAR_MIPMAP_LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			break;
		}
	}

	min_filter = filter;
}

void ResourceTexture::SetMagFilter(TextureMagFilter filter)
{
	for (Texture& texture : textures)
	{
		glBindTexture(GL_TEXTURE_2D, texture.gl_texture);
		switch (filter)
		{
		case TextureMagFilter::NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;
		case TextureMagFilter::LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;
		}
	}

	mag_filter = filter;
}

void ResourceTexture::SetWrap(TextureWrap wrap)
{
	for (Texture& texture : textures)
	{
		glBindTexture(GL_TEXTURE_2D, texture.gl_texture);
		switch (wrap)
		{
		case TextureWrap::REPEAT:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			break;
		case TextureWrap::CLAMP_TO_EDGE:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			break;
		case TextureWrap::CLAMP_TO_BORDER:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			break;
		case TextureWrap::MIRROR_REPEAT:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			break;
		case TextureWrap::MIRROR_CLAMP_TO_EDGE:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRROR_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRROR_CLAMP_TO_EDGE);
			break;
		}
	}

	texture_wrap = wrap;
}

TextureMinFilter ResourceTexture::GetMinFilter() const
{
	return min_filter;
}

TextureMagFilter ResourceTexture::GetMagFilter() const
{
	return mag_filter;
}

TextureWrap ResourceTexture::GetWrap() const
{
	return texture_wrap;
}