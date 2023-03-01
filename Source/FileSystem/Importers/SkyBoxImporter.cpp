#include "SkyBoxImporter.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "FileSystem/ModuleFileSystem.h"
#include "EngineLog.h"
#include "FileSystem/Json.h"

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

	std::vector<std::shared_ptr<Resource>> faces(6);

	for(int i = 0; i < facesPaths.size(); ++i)
	{
		faces[i] = App->resources->RequestResource(facesPaths[i]);
	}

	resource->SetTextures(faces);

	char* buffer{};
	unsigned int size;
	Save(resource, buffer, size);
	App->fileSystem->Save((resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), buffer, size);

	delete buffer;
}

void SkyBoxImporter::Save(const std::shared_ptr<ResourceSkyBox>& resource, char*& fileBuffer, unsigned int& size)
{
	size = sizeof(UID) * resource->GetTextures().size();
	char* cursor = new char[size] {};
	fileBuffer = cursor;
	unsigned int bytes = size;

	std::vector<UID> texturesUIDs;
	texturesUIDs.reserve(resource->GetTextures().size());
	for(int i = 0; i < resource->GetTextures().size(); i++) 
	{
		texturesUIDs.push_back(resource->GetTextures()[i]->GetUID());
	}
	memcpy(cursor, &(texturesUIDs[0]), bytes);
}

void SkyBoxImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceSkyBox> resource)
{
	UID* texturesPointer = new UID[6];
	unsigned int bytes = sizeof(UID) * 6;
	memcpy(texturesPointer, fileBuffer, bytes);
	std::vector<UID> texturesUIDs(texturesPointer, texturesPointer + 6);
	std::vector<std::shared_ptr<Resource>> textures;
	textures.reserve(6);
	for (int i = 0; i < texturesUIDs.size(); i++)
	{
		textures.push_back(App->resources->SearchResource<Resource>(texturesUIDs[i]));
	}

	resource->SetTextures(textures);

	delete[] texturesPointer;
}