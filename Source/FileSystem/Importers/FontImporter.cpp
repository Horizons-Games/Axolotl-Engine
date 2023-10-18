#include "StdAfx.h"
#include "FontImporter.h"
#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"
#include "Resources/ResourceFont.h"

FontImporter::FontImporter()
{
}

FontImporter::~FontImporter()
{
}

void FontImporter::Import(const char* filePath, std::shared_ptr<ResourceFont> resource)
{
	char* buffer{};
	unsigned int size;
	if (resource->ImportFont(filePath))
	{
		Save(resource, buffer, size);
		App->GetModule<ModuleFileSystem>()->Save(
			(resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), buffer, size);
	}		
	delete[] buffer;
}

void FontImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceFont> resource)
{
	if (resource->IsCharacterEmpty())
	{
		resource->storeFont.clear();
		int num = 0;

		while (num < 128)
		{
			unsigned int headerTypeUnsignetInt[5];
			int headerTypeInt[2];
			memcpy(headerTypeUnsignetInt, fileBuffer, sizeof(headerTypeUnsignetInt));
			fileBuffer += sizeof(headerTypeUnsignetInt);
			memcpy(headerTypeInt, fileBuffer, sizeof(headerTypeInt));
			fileBuffer += sizeof(headerTypeInt);
			if (headerTypeUnsignetInt[4] != 0)
			{
				unsigned char* pixelsPointer = new unsigned char[headerTypeUnsignetInt[4]];
				memcpy(pixelsPointer, fileBuffer, sizeof(unsigned char) * headerTypeUnsignetInt[4]);
				std::vector<uint8_t> pixel(pixelsPointer, pixelsPointer + headerTypeUnsignetInt[4]);

				StoreFontInPixels newstore = {
					headerTypeUnsignetInt[0], headerTypeUnsignetInt[1], headerTypeUnsignetInt[2], headerTypeInt[0],
					headerTypeInt[1],		  headerTypeUnsignetInt[3], headerTypeUnsignetInt[4], pixel

				};
				resource->storeFont.push_back(newstore);
				fileBuffer += sizeof(unsigned char) * headerTypeUnsignetInt[4];

				delete[] pixelsPointer;
			}
			else
			{
				std::vector<uint8_t> pixel;
				StoreFontInPixels newstore = {
					headerTypeUnsignetInt[0], headerTypeUnsignetInt[1], headerTypeUnsignetInt[2], headerTypeInt[0],
					headerTypeInt[1],		  headerTypeUnsignetInt[3], headerTypeUnsignetInt[4], pixel

				};
				resource->storeFont.push_back(newstore);
			}
			num++;
		}
	}	
}
	

void FontImporter::Save(const std::shared_ptr<ResourceFont>& resource, char*& fileBuffer, unsigned int& size)
{
	size = 0;	
	for (StoreFontInPixels it : resource->storeFont)
	{
		
		unsigned int headerTypeUnsignetInt[5] = { it.symbol, it.width, it.height, it.advance_x, it.size };
		int headerTypeInt[2] = { it.left, it.top };

		size += sizeof(headerTypeUnsignetInt) + sizeof(headerTypeInt) + sizeof(unsigned char) * it.size;
	}
	char* cursor = new char[size];
	
	fileBuffer = cursor;
	for (const StoreFontInPixels &it : resource->storeFont)
	{
		unsigned int headerTypeUnsignetInt[5] = { it.symbol, it.width, it.height, it.advance_x, it.size };
		int headerTypeInt[2] = { it.left, it.top };
		
		unsigned int bytes = sizeof(headerTypeUnsignetInt);		
		memcpy(cursor, headerTypeUnsignetInt, bytes);		
		cursor += bytes;
		bytes = sizeof(headerTypeInt);
		memcpy(cursor, headerTypeInt, bytes);
		cursor += bytes;
		if (it.size != 0)
		{
			bytes = sizeof(unsigned char) * it.size;
			memcpy(cursor, &(it.pixels)[0], bytes);				
			cursor += bytes;			
		}		
	}	
}