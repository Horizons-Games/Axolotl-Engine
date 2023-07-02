#include "StdAfx.h"

#include "MaterialImporter.h"
#include "Application.h"
#include "DataModels/Resources/ResourceTexture.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"
#include "FileSystem/Json.h"
MaterialImporter::MaterialImporter()
{
}

MaterialImporter::~MaterialImporter()
{
}

void MaterialImporter::Import(const char* filePath, std::shared_ptr<ResourceMaterial> resource)
{
	char* bufferPaths;
	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();

	{
		fileSystem->Load(filePath, bufferPaths);
		rapidjson::Document doc;
		Json mat(doc, doc);
		mat.fromBuffer(bufferPaths);
		delete bufferPaths;

		resource->LoadPaths(mat);
	}
	
	// Load Meta to Update Load path options
	std::string metaPath = resource->GetAssetsPath() + META_EXTENSION;
	char* metaBuffer = {};
	rapidjson::Document doc;

	fileSystem->Load(metaPath.c_str(), metaBuffer);
	Json meta(doc, doc);
	meta.fromBuffer(metaBuffer);

	delete metaBuffer;

	resource->SavePaths(meta);

	rapidjson::StringBuffer bufferMeta;
	meta.toBuffer(bufferMeta);
	App->GetModule<ModuleFileSystem>()->Save(
		metaPath.c_str(), bufferMeta.GetString(), (unsigned int) bufferMeta.GetSize());

	char* buffer{};
	unsigned int size;

	Save(resource, buffer, size);
	fileSystem->Save((resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), buffer, size);

	delete buffer;
}

void MaterialImporter::Save(const std::shared_ptr<ResourceMaterial>& resource, char*& fileBuffer, unsigned int& size)
{
	UID metalicOrSpecularUID;

	switch (resource->GetShaderType())
	{
		case 0:

			if (resource->GetMetallic())
			{
				metalicOrSpecularUID = resource->GetMetallic()->GetUID();
			}
			else
			{
				metalicOrSpecularUID = 0;
			}

			break;
		case 1:

			if (resource->GetSpecular())
			{
				metalicOrSpecularUID = resource->GetSpecular()->GetUID();
			}
			else
			{
				metalicOrSpecularUID = 0;
			}

			break;
	}

	UID texturesUIDs[5] = { resource->GetDiffuse() ? resource->GetDiffuse()->GetUID() : 0,
							resource->GetNormal() ? resource->GetNormal()->GetUID() : 0,
							resource->GetOcclusion() ? resource->GetOcclusion()->GetUID() : 0,
							metalicOrSpecularUID,
							resource->GetEmission() ? resource->GetEmission()->GetUID() : 0};

	LoadOptionsMaterial options = resource->GetLoadOptions();

	size = sizeof(texturesUIDs) + sizeof(options);

	char* cursor = new char[size];

	fileBuffer = cursor;

	unsigned int bytes = sizeof(texturesUIDs);
	memcpy(cursor, texturesUIDs, bytes);

	cursor += bytes;

	bytes = sizeof(options);
	memcpy(cursor, &options, bytes);
}

void MaterialImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceMaterial> resource)
{
#ifndef ENGINE

	UID texturesUIDs[5];
	memcpy(texturesUIDs, fileBuffer, sizeof(texturesUIDs));

	fileBuffer += sizeof(texturesUIDs);

	LoadOptionsMaterial options;
	memcpy(&options, fileBuffer, sizeof(LoadOptionsMaterial));

	fileBuffer += sizeof(LoadOptionsMaterial);

	if (texturesUIDs[0] != 0)
	{
		resource->SetDiffuse(App->GetModule<ModuleResources>()->SearchResource<ResourceTexture>(texturesUIDs[0]));
	}

	if (texturesUIDs[1] != 0)
	{
		resource->SetNormal(App->GetModule<ModuleResources>()->SearchResource<ResourceTexture>(texturesUIDs[1]));
	}

	if (texturesUIDs[2] != 0)
	{
		resource->SetOcclusion(App->GetModule<ModuleResources>()->SearchResource<ResourceTexture>(texturesUIDs[2]));
	}

	if (texturesUIDs[3] != 0)
	{
		switch (*shaderType)
		{
			case 0:

				resource->SetMetallic(
					App->GetModule<ModuleResources>()->SearchResource<ResourceTexture>(texturesUIDs[3]));

				break;

			case 1:

				resource->SetSpecular(
					App->GetModule<ModuleResources>()->SearchResource<ResourceTexture>(texturesUIDs[3]));

				break;
		}
	}

	if (texturesUIDs[4] != 0)
	{
		resource->SetEmission(App->GetModule<ModuleResources>()->SearchResource<ResourceTexture>(texturesUIDs[4]));
	}
#endif
}
