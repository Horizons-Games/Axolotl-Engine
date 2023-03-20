#include "SkyBoxImporter.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/Json.h"

SkyBoxImporter::SkyBoxImporter()
{
}

SkyBoxImporter::~SkyBoxImporter()
{
}

void SkyBoxImporter::Import(const char* filePath, std::shared_ptr<ResourceSkyBox> resource)
{
	char* bufferFile;

	App->fileSystem->Load(resource->GetAssetsPath().c_str(), bufferFile);
	
	rapidjson::Document doc;
	Json Json(doc, doc);

	Json.fromBuffer(bufferFile);
	delete bufferFile;
	std::vector<std::string> facesPaths(6);
	facesPaths[0] = Json["right"];
	facesPaths[1] = Json["left"];
	facesPaths[2] = Json["top"];
	facesPaths[3] = Json["bottom"];
	facesPaths[4] = Json["back"];
	facesPaths[5] = Json["front"];

	std::vector<UID> faces(6);

	for(int i = 0; i < facesPaths.size(); ++i)
	{
		UID resourceTexture = App->resources->ImportResource(facesPaths[i]);
		faces[i] = resourceTexture;
	}

	resource->SetTexturesUIDs(faces);

	char* buffer{};
	unsigned int size;
	Save(resource, buffer, size);
	App->fileSystem->Save((resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), buffer, size);

	delete buffer;
}

void SkyBoxImporter::Save(const std::shared_ptr<ResourceSkyBox>& resource, char*& fileBuffer, unsigned int& size)
{
	size = sizeof(UID) * 6;
	char* cursor = new char[size] {};
	fileBuffer = cursor;
	unsigned int bytes = size;
	memcpy(cursor, &(resource->GetTexturesUIDs()[0]), bytes);
}

void SkyBoxImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceSkyBox> resource)
{
	UID* texturesPointer = new UID[6];
	unsigned int bytes = sizeof(UID) * 6;
	memcpy(texturesPointer, fileBuffer, bytes);
	std::vector<UID> textures(texturesPointer, texturesPointer + 6);
	resource->SetTexturesUIDs(textures);

	delete[] texturesPointer;
}