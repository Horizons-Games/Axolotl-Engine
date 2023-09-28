#pragma once
#include "Resource.h"
//#include "Math/float2.h"
//#include "Math/float3.h"

struct Character
{
	unsigned int TextureID; // ID handle of the glyph texture
	float2 Size;			// Size of glyph
	float2 Bearing;			// Offset from baseline to left/top of glyph
	unsigned int Advance;	// Horizontal offset to advance to next glyph	
};


struct StoreFontInPixels 
{
	unsigned char symbol;
	unsigned int width;
	unsigned int height;
	int left;
	int top;
	int advance_x;	
	unsigned int size;
	std::vector <uint8_t> pixels;	
};

class ResourceFont : virtual public Resource
{
public:
	
	ResourceFont(UID resourceUID,
		const std::string& fileName,
		const std::string& assetsPath,
		const std::string& libraryPath);
	virtual ~ResourceFont() override;
	ResourceType GetType() const override;
	bool ImportFont(std::string nameFount);
	bool LoadCharacter();
	std::map<char, Character> GetCharacters();
	bool IsCharacterEmpty();
	std::vector<StoreFontInPixels> storeFont;
	void SaveImporterOptions(Json& meta) override{};
	void LoadImporterOptions(Json& meta) override{};

	void SaveLoadOptions(Json& meta) override{};
	void LoadLoadOptions(Json& meta) override{};

protected:
	void InternalLoad() override;
	void InternalUnload() override;

private:
	std::map<char, Character> characters;
};

inline ResourceType ResourceFont::GetType() const
{
	return ResourceType::Font;
}
