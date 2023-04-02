#include "SkyBoxImporter.h"

#include "Application.h"
#include "DataModels/Resources/ResourceTexture.h"
#include "FileSystem/Json.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"

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

	std::vector<std::shared_ptr<ResourceTexture>> faces(6);

	for (int i = 0; i < facesPaths.size(); ++i)
	{
		faces[i] = std::dynamic_pointer_cast<ResourceTexture>(App->resources->ImportResource(facesPaths[i]));
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
#ifdef ENGINE
	// Open Meta
	std::string metaPath = resource->GetAssetsPath() + META_EXTENSION;
	char* metaBuffer = {};
	App->fileSystem->Load(metaPath.c_str(), metaBuffer);
	rapidjson::Document doc;
	Json meta(doc, doc);
	meta.fromBuffer(metaBuffer);
	delete metaBuffer;
#endif

	size = (unsigned int) (sizeof(UID) * resource->GetTextures().size());
	char* cursor = new char[size]{};
	fileBuffer = cursor;
	unsigned int bytes = size;

	std::vector<UID> texturesUIDs;
	texturesUIDs.reserve(resource->GetTextures().size());
	for (int i = 0; i < resource->GetTextures().size(); i++)
	{
#ifdef ENGINE
		// Update Meta
		Json jsonTexture = meta["TexturesAssetPaths"];
		jsonTexture[i] = resource->GetTextures()[i]->GetAssetsPath().c_str();
#endif
		texturesUIDs.push_back(resource->GetTextures()[i]->GetUID());
	}
	memcpy(cursor, &(texturesUIDs[0]), bytes);

#ifdef ENGINE
	// Save Meta
	rapidjson::StringBuffer buffer;
	meta.toBuffer(buffer);
	App->fileSystem->Save(metaPath.c_str(), buffer.GetString(), (unsigned int) buffer.GetSize());
#endif
}

void SkyBoxImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceSkyBox> resource)
{
#ifdef ENGINE
	// Open Meta
	std::string metaPath = resource->GetAssetsPath() + META_EXTENSION;
	char* metaBuffer = {};
	App->fileSystem->Load(metaPath.c_str(), metaBuffer);
	rapidjson::Document doc;
	Json meta(doc, doc);
	meta.fromBuffer(metaBuffer);
	delete metaBuffer;
#endif

	int size = 6;
	std::vector<std::shared_ptr<ResourceTexture>> textures;
	textures.reserve(size);

#ifdef ENGINE
	Json jsonTextures = meta["TexturesAssetPaths"];
	for (int i = 0; i < size; i++)
	{
		std::string texturePath = jsonTextures[i];
		textures.push_back(App->resources->RequestResource<ResourceTexture>(texturePath));
	}
#else
	UID* texturesPointer = new UID[size];
	unsigned int bytes = sizeof(UID) * size;
	memcpy(texturesPointer, fileBuffer, bytes);
	std::vector<UID> texturesUIDs(texturesPointer, texturesPointer + size);
	delete[] texturesPointer;
	for (int i = 0; i < texturesUIDs.size(); i++)
	{
		textures.push_back(App->resources->SearchResource<ResourceTexture>(texturesUIDs[i]));
	}
#endif

	resource->SetTextures(textures);
}