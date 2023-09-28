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
	/*ENGINE_LOG("Import font from %s", filePath);*/
	char* buffer{};
	unsigned int size;
	if (resource.get()->ImportFont(filePath)) {		
		Save(resource, buffer, size);
		App->GetModule<ModuleFileSystem>()->Save(
			(resource.get()->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), buffer, size);
		/*App->fileSystem->Save((resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), buffer, size);*/		
	}	
	/*buffer = nullptr;
	delete[] buffer;*/
}

void FontImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceFont> resource)
{
	if (resource.get()->IsCharacterEmpty())
	{
		resource.get()->storeFont.clear();
		int num = 0;

		while (num < 128)
		{
			unsigned int header[5];
			int header2[2];
			memcpy(header, fileBuffer, sizeof(header));
			fileBuffer += sizeof(header);
			memcpy(header2, fileBuffer, sizeof(header2));
			fileBuffer += sizeof(header2);
			if (header[4] != 0)
			{
				unsigned char* pixelsPointer = new unsigned char[header[4]];
				memcpy(pixelsPointer, fileBuffer, sizeof(unsigned char) * header[4]);
				std::vector<uint8_t> pixel(pixelsPointer, pixelsPointer + header[4]);

				StoreFontInPixels newstore = { header[0],  header[1], header[2], header2[0],
											   header2[1], header[3], header[4], pixel

				};
				resource.get()->storeFont.push_back(newstore);
				fileBuffer += sizeof(unsigned char) * header[4];

				delete[] pixelsPointer;
			}
			else
			{
				std::vector<uint8_t> pixel;
				StoreFontInPixels newstore = { header[0],  header[1], header[2], header2[0],
											   header2[1], header[3], header[4], pixel

				};
				resource.get()->storeFont.push_back(newstore);
			}
			num++;
		}
	}	
}
	

void FontImporter::Save(const std::shared_ptr<ResourceFont>& resource, char*& fileBuffer, unsigned int& size)
{
	size = 0;	
	for (StoreFontInPixels it : resource.get()->storeFont)
	{
		
			unsigned int header[5] =
			{
				it.symbol,
				it.width,
				it.height,				
				it.advance_x,
				it.size
			};
		int header2[2] = { it.left, it.top };

			size += sizeof(header) + sizeof(header2) + sizeof(unsigned char) * it.size;
	}
	char* cursor = new char[size];
	
	fileBuffer = cursor;
	for (StoreFontInPixels it : resource.get()->storeFont)
	{
		unsigned int header[5] = { it.symbol, it.width, it.height, it.advance_x, it.size };
		int header2[2] = { it.left, it.top };
		
		unsigned int bytes = sizeof(header);		
		memcpy(cursor, header, bytes);		
		cursor += bytes;
		bytes = sizeof(header2);
		memcpy(cursor, header2, bytes);
		cursor += bytes;
		if (it.size != 0)
		{
			bytes = sizeof(unsigned char) * it.size;
			memcpy(cursor, &(it.pixels)[0], bytes);				
			cursor += bytes;			
		}		
	}	
}