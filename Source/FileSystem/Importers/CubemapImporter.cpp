#include "CubemapImporter.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/Json.h"

#include "DataModels/Resources/ResourceCubemap.h"
#include "DataModels/Resources/ResourceTexture.h"

CubemapImporter::CubemapImporter()
{
}

CubemapImporter::~CubemapImporter()
{
}

void CubemapImporter::Import(const char* filePath, std::shared_ptr<ResourceCubemap> resource)
{
	char* bufferFile;

	App->fileSystem->Load(resource->GetAssetsPath().c_str(), bufferFile);

	rapidjson::Document doc;
	Json Json(doc, doc);

	Json.fromBuffer(bufferFile);
	delete bufferFile;
	std::string texturePath = Json["texture"];

	std::shared_ptr<ResourceTexture> hdrTexture = std::dynamic_pointer_cast<ResourceTexture>(
		App->resources->ImportResource(texturePath));

	resource->SetHDRTexture(hdrTexture);

	char* buffer{};
	unsigned int size;
	Save(resource, buffer, size);
	App->fileSystem->Save((resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), buffer, size);

	delete buffer;
}

void CubemapImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceCubemap> resource)
{
#ifdef ENGINE
	//Open Meta
	std::string metaPath = resource->GetAssetsPath() + META_EXTENSION;
	char* metaBuffer = {};
	App->fileSystem->Load(metaPath.c_str(), metaBuffer);
	rapidjson::Document doc;
	Json meta(doc, doc);
	meta.fromBuffer(metaBuffer);
	delete metaBuffer;
#endif

	std::shared_ptr<ResourceTexture> hdrTexture;

#ifdef  ENGINE
	Json jsonTexture = meta["TextureAssetPath"];

	hdrTexture = App->resources->RequestResource<ResourceTexture>(jsonTexture);
#else
	UID* texturePointer = new UID;
	unsigned int bytes = sizeof(UID);
	memcpy(texturePointer, fileBuffer, bytes);
	hdrTexture = App->resources->SearchResource<ResourceTexture>(texturePointer);
	delete[] texturePointer;
#endif

	resource->SetHDRTexture(hdrTexture);
}

void CubemapImporter::Save(const std::shared_ptr<ResourceCubemap>& resource, char*& fileBuffer, unsigned int& size)
{
#ifdef ENGINE
	//Open Meta
	std::string metaPath = resource->GetAssetsPath() + META_EXTENSION;
	char* metaBuffer = {};
	App->fileSystem->Load(metaPath.c_str(), metaBuffer);
	rapidjson::Document doc;
	Json meta(doc, doc);
	meta.fromBuffer(metaBuffer);
	delete metaBuffer;
#endif

	size = (unsigned int)(sizeof(UID));
	char* cursor = new char[size] {};
	fileBuffer = cursor;
	unsigned int bytes = size;

	UID textureUID;
	textureUID = resource->GetHDRTexture()->GetUID();
	memcpy(cursor, &textureUID, bytes);

#ifdef ENGINE
	//Save Meta
	rapidjson::StringBuffer buffer;
	meta.toBuffer(buffer);
	App->fileSystem->Save(metaPath.c_str(), buffer.GetString(), (unsigned int)buffer.GetSize());
#endif
}
