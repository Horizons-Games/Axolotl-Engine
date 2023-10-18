#include "StdAfx.h"
#include "ResourceFont.h"
//#include "MathGeoLib.h"
#include "FileSystem/Json.h"
#include "GL/glew.h"
#include <ft2build.h>
#include FT_FREETYPE_H

ResourceFont::ResourceFont(UID resourceUID, const std::string& fileName, const std::string& assetsPath, const std::string& libraryPath)
	:Resource(resourceUID, fileName, assetsPath, libraryPath)
{
}

ResourceFont::~ResourceFont()
{
	Unload();
}

void ResourceFont::InternalLoad()
{
}

void ResourceFont::InternalUnload()
{

}

bool ResourceFont::ImportFont(std::string nameFount)
{
	FT_Library ft;	
	if (FT_Init_FreeType(&ft))
	{		
		return false;
	}
	std::string font_name = nameFount;
	if (font_name.empty())
	{		
		return false;
	}
	FT_Face face;
	if (FT_New_Face(ft, font_name.c_str(), 0, &face)) 
	{		
		return false;
	}
	else 
	{		
		FT_Set_Pixel_Sizes(face, 0, 48);

		for (unsigned char c = 0; c < 128; c++)
		{			
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				LOG_INFO("ERROR::FREETYTPE: Failed to load Glyph");
				continue;
			}			
			unsigned int sizeBuffer = face->glyph->bitmap.width * face->glyph->bitmap.rows;
			std::vector<uint8_t> pixel(face->glyph->bitmap.buffer, face->glyph->bitmap.buffer + sizeBuffer);
			StoreFontInPixels store = { c,
										face->glyph->bitmap.width,
										face->glyph->bitmap.rows,
										face->glyph->bitmap_left,
										face->glyph->bitmap_top,
										static_cast<unsigned int>(face->glyph->advance.x),
										sizeBuffer,
										pixel };
			storeFont.push_back(store);
		}			
	}
	
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	return true;
}


	bool ResourceFont::LoadCharacter()
	{		
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);		
		for (StoreFontInPixels store : storeFont)
		{			
			std::vector<uint8_t> aux = store.pixels;
			std::reverse(aux.begin(), aux.end());
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D,
						 0,
						 GL_RED,
						 store.width,
						 store.height,
						 0,
						 GL_RED,
						 GL_UNSIGNED_BYTE,
						 (store.size != 0) ? &(aux[0]) : nullptr

			);			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			
			Character character = {
				texture, float2(store.width, store.height), float2(store.left, store.top), store.advance_x
			};
			characters.insert(std::pair<char, Character>(store.symbol, character));
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		return true;
	}

	std::map<char, Character> ResourceFont::GetCharacters()
	{
		if (characters.empty()) 
		{
			this->LoadCharacter();
		}		
		return characters;
	}

	bool ResourceFont::IsCharacterEmpty()
	{
		return characters.empty();
	}
	
	