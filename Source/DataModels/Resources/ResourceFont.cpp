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
	// FreeType
	// --------
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
	{
		/*ENGINE_LOG("ERROR::FREETYPE: Could not init FreeType Library");*/
		return false;
	}

	// find path to font
	//std::string font_name = FileSystem::getPath("resources/fonts/Antonio-Bold.ttf");
	std::string font_name = nameFount;
	if (font_name.empty())
	{
		/*ENGINE_LOG("ERROR::FREETYPE: Failed to load font_name");*/
		return false;
	}

	// load font as face
	FT_Face face;
	if (FT_New_Face(ft, font_name.c_str(), 0, &face)) 
	{
		/*ENGINE_LOG("ERROR::FREETYPE: Failed to load font");*/
		return false;
	}
	else 
	{
		// set size to load glyphs as
		FT_Set_Pixel_Sizes(face, 0, 48);

		// disable byte-alignment restriction
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// load first 128 characters of ASCII set
		for (unsigned char c = 0; c < 128; c++)
		{
			// Load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				LOG_INFO("ERROR::FREETYTPE: Failed to load Glyph");
				continue;
			}			
			unsigned int sizeBuffer = face->glyph->bitmap.width * face->glyph->bitmap.rows;
			std::vector<uint8_t> pixel(face->glyph->bitmap.buffer, face->glyph->bitmap.buffer + sizeBuffer);
			StoreFontInPixels store = {
				c,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				face->glyph->bitmap_left,
				face->glyph->bitmap_top,
				static_cast<unsigned int>(face->glyph->advance.x),
				sizeBuffer,
				pixel
			};
			/*LOG_INFO("n %c", c);
			LOG_INFO("width %u", face->glyph->bitmap.width);
			LOG_INFO("rows %u", face->glyph->bitmap.rows);
			LOG_INFO("left %u", face->glyph->bitmap_left);
			LOG_INFO("top %u", face->glyph->bitmap_top);
			LOG_INFO("advance in x %u", static_cast<unsigned int>(face->glyph->advance.x));
			LOG_INFO("size %u", face->glyph->bitmap.width * face->glyph->bitmap.rows);
			LOG_INFO("size 2 %u", (unsigned int) sizeof((uint8_t) face->glyph->bitmap.buffer));*/

			storeFont.push_back(store);
		}
			
	}
	// destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	return true;
}


	bool ResourceFont::LoadCharacter()
	{
		//disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		/*ENGINE_LOG("load textures")*/
		for (StoreFontInPixels store : storeFont)
		{			
			std::vector<uint8_t> aux = store.pixels;
			std::reverse(aux.begin(), aux.end());
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				store.width,
				store.height,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				(store.size!=0)?&(aux[0]):nullptr
				
			);
			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// now store character for later use
			Character character = {
				texture,
				float2(store.width, store.height),
				float2(store.left, store.top),
				store.advance_x
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
	
	