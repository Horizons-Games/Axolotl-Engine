#include "StdAfx.h"

#include "CubemapImporter.h"

#include "Application.h"
#include "FileSystem/Json.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"

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

	App->GetModule<ModuleFileSystem>()->Load(resource->GetAssetsPath().c_str(), bufferFile);

	rapidjson::Document doc;
	Json Json(doc, doc);

	Json.fromBuffer(bufferFile);
	delete bufferFile;
	std::string texturePath = Json["texture"];

	std::shared_ptr<ResourceTexture> hdrTexture =
		std::dynamic_pointer_cast<ResourceTexture>(App->GetModule<ModuleResources>()->ImportResource(texturePath));

	resource->SetHDRTexture(hdrTexture);

	char* buffer{};
	unsigned int size;
	Save(resource, buffer, size);
	App->GetModule<ModuleFileSystem>()->Save(
		(resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), buffer, size);

	delete buffer;
}

void CubemapImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceCubemap> resource)
{
#ifdef ENGINE
	// Open Meta
	std::string metaPath = resource->GetAssetsPath() + META_EXTENSION;
	char* metaBuffer = {};
	App->GetModule<ModuleFileSystem>()->Load(metaPath.c_str(), metaBuffer);
	rapidjson::Document doc;
	Json meta(doc, doc);
	meta.fromBuffer(metaBuffer);
	delete metaBuffer;
#endif

	std::shared_ptr<ResourceTexture> hdrTexture;

#ifdef ENGINE
	Json jsonTexture = meta["TextureAssetPath"];

	hdrTexture = App->GetModule<ModuleResources>()->RequestResource<ResourceTexture>(jsonTexture);
#else
	UID* texturePointer = new UID;
	unsigned int bytes = sizeof(UID);
	memcpy(texturePointer, fileBuffer, bytes);
	UID texture = *texturePointer;
	delete texturePointer;
	hdrTexture = App->GetModule<ModuleResources>()->SearchResource<ResourceTexture>(texture);
#endif

	resource->SetHDRTexture(hdrTexture);
}

void CubemapImporter::Save(const std::shared_ptr<ResourceCubemap>& resource, char*& fileBuffer, unsigned int& size)
{
#ifdef ENGINE
	// Open Meta
	std::string metaPath = resource->GetAssetsPath() + META_EXTENSION;
	char* metaBuffer = {};
	App->GetModule<ModuleFileSystem>()->Load(metaPath.c_str(), metaBuffer);
	rapidjson::Document doc;
	Json meta(doc, doc);
	meta.fromBuffer(metaBuffer);
	delete metaBuffer;
#endif

	size = (unsigned int) (sizeof(UID));
	char* cursor = new char[size]{};
	fileBuffer = cursor;
	unsigned int bytes = size;

	UID textureUID;
	textureUID = resource->GetHDRTexture()->GetUID();
#ifdef ENGINE
	Json jsonTexture = meta["TextureAssetPath"];
	jsonTexture = resource->GetHDRTexture()->GetAssetsPath().c_str();
#endif

	memcpy(cursor, &textureUID, bytes);

#ifdef ENGINE
	// Save Meta
	rapidjson::StringBuffer buffer;
	meta.toBuffer(buffer);
	App->GetModule<ModuleFileSystem>()->Save(metaPath.c_str(), buffer.GetString(), (unsigned int) buffer.GetSize());
#endif
}
