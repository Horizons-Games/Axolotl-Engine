#include "MaterialImporter.h"
#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"
#include "DataModels/Resources/ResourceTexture.h"
#include <iostream>

MaterialImporter::MaterialImporter()
{
}

MaterialImporter::~MaterialImporter()
{
}

void MaterialImporter::Import(const char* filePath, std::shared_ptr<ResourceMaterial> resource)
{
	char* bufferPaths;

	App->fileSystem->Load(filePath, bufferPaths);

	unsigned int header[4];
	memcpy(header, bufferPaths, sizeof(header));

	bufferPaths += sizeof(header);

	std::vector<std::shared_ptr<ResourceTexture>> resourceTexture;

	for (int i = 0; i < 4; ++i)
	{
		char* pathPointer = new char[header[i]];
		memcpy(pathPointer, bufferPaths, header[i]);
		std::string path(pathPointer, pathPointer + header[i]);

		if (!path.empty()) 
		{
			resourceTexture.push_back(std::dynamic_pointer_cast<ResourceTexture>(App->resources->ImportResource(path)));
		}
		else
		{
			resourceTexture.push_back(0);
		}

		bufferPaths += header[i];

		delete[] pathPointer;
	}

	if (resourceTexture[0] != 0)
	{
		resource->SetDiffuse(resourceTexture[0]);
	}
	
	if (resourceTexture[1] != 0)
	{
		resource->SetNormal(resourceTexture[1]);
	}
	
	if (resourceTexture[2] != 0)
	{
		resource->SetOcclusion(resourceTexture[2]);
	}
	
	/*if (resourceTexture[3] != 0)
	{
		resource->SetSpecular(resourceTexture[3]);
	}*/

	if (resourceTexture[3] != 0)
	{
		resource->SetMetallicMap(resourceTexture[3]);
	}

	char* buffer{};
	unsigned int size;
	Save(resource, buffer, size);
	App->fileSystem->Save((resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), buffer, size);

	delete buffer;
}

void MaterialImporter::Save(const std::shared_ptr<ResourceMaterial>& resource, char*& fileBuffer, unsigned int& size)
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

	meta["DiffuseAssetPath"] = resource->GetDiffuse() ? resource->GetDiffuse()->GetAssetsPath().c_str() : "";
	meta["NormalAssetPath"] = resource->GetNormal() ? resource->GetNormal()->GetAssetsPath().c_str() : "";
	meta["OcclusionAssetPath"] = resource->GetOcclusion() ? resource->GetOcclusion()->GetAssetsPath().c_str() : "";
	//meta["SpecularAssetPath"] = resource->GetSpecular() ? resource->GetSpecular()->GetAssetsPath().c_str() : "";
	meta["MetallicAssetPath"] = resource->GetMetallicMap() ? resource->GetMetallicMap()->GetAssetsPath().c_str() : "";

	rapidjson::StringBuffer buffer;
	meta.toBuffer(buffer);
	App->fileSystem->Save(metaPath.c_str(), buffer.GetString(), (unsigned int)buffer.GetSize());
#endif

	UID texturesUIDs[4] =
	{
		resource->GetDiffuse() ? resource->GetDiffuse()->GetUID() : 0,
		resource->GetNormal() ? resource->GetNormal()->GetUID() : 0,
		resource->GetOcclusion() ? resource->GetOcclusion()->GetUID() : 0,
		//resource->GetSpecular() ? resource->GetSpecular()->GetUID() : 0
		resource->GetMetallicMap() ? resource->GetMetallicMap()->GetUID() : 0
	};

	float4 diffuseColor[1] = { resource->GetDiffuseColor() };
	//float3 specularColor[1] = { resource->GetSpecularColor() };
	

	size = sizeof(texturesUIDs) + sizeof(diffuseColor) + sizeof(specularColor)  + sizeof(float) * 2 + sizeof(bool);

	char* cursor = new char[size];

	fileBuffer = cursor;

	unsigned int bytes = sizeof(texturesUIDs);
	memcpy(cursor, texturesUIDs, bytes);

	cursor += bytes;

	bytes = sizeof(diffuseColor);
	memcpy(cursor, diffuseColor, bytes);

	cursor += bytes;

	bytes = sizeof(specularColor);
	memcpy(cursor, specularColor, bytes);

	cursor += bytes;

	/*bytes = sizeof(float);
	memcpy(cursor, &resource->GetShininess(), bytes);

	cursor += bytes;*/

	bytes = sizeof(float);
	memcpy(cursor, &resource->GetNormalStrength(), bytes);

	cursor += bytes;

	bytes = sizeof(bool);
	memcpy(cursor, &resource->GetTransparent(), bytes);
}

void MaterialImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceMaterial> resource)
{
	UID texturesUIDs[4];
	memcpy(texturesUIDs, fileBuffer, sizeof(texturesUIDs));

#ifdef ENGINE
	//Open Meta
	std::string metaPath = resource->GetAssetsPath() + META_EXTENSION;
	char* metaBuffer = {};
	App->fileSystem->Load(metaPath.c_str(), metaBuffer);
	rapidjson::Document doc;
	Json meta(doc, doc);
	meta.fromBuffer(metaBuffer);
	delete metaBuffer;

	std::string assetPath = meta["DiffuseAssetPath"];
	if (assetPath != "") resource->SetDiffuse(App->resources->RequestResource<ResourceTexture>(assetPath));
	assetPath = meta["NormalAssetPath"];
	if (assetPath != "") resource->SetNormal(App->resources->RequestResource<ResourceTexture>(assetPath));
	assetPath = meta["OcclusionAssetPath"];
	if (assetPath != "") resource->SetOcclusion(App->resources->RequestResource<ResourceTexture>(assetPath));
	/*assetPath = meta["SpecularAssetPath"];
	if (assetPath != "") resource->SetSpecular(App->resources->RequestResource<ResourceTexture>(assetPath));*/
	assetPath = meta["MetallicAssetPath"];
	if (assetPath != "") resource->SetMetallicMap(App->resources->RequestResource<ResourceTexture>(assetPath));
#else
	
	if (texturesUIDs[0] != 0) resource->SetDiffuse(App->resources->SearchResource<ResourceTexture>(texturesUIDs[0]));
	if (texturesUIDs[1] != 0) resource->SetNormal(App->resources->SearchResource<ResourceTexture>(texturesUIDs[1]));
	if (texturesUIDs[2] != 0) resource->SetOcclusion(App->resources->SearchResource<ResourceTexture>(texturesUIDs[2]));
	//if (texturesUIDs[3] != 0) resource->SetSpecular(App->resources->SearchResource<ResourceTexture>(texturesUIDs[3]));
	if (texturesUIDs[3] != 0) resource->SetMetallicMap(App->resources->SearchResource<ResourceTexture>(texturesUIDs[3]));

#endif

	fileBuffer += sizeof(texturesUIDs);

	float4 difuseColor[1];
	memcpy(difuseColor, fileBuffer, sizeof(difuseColor));
	resource->SetDiffuseColor(difuseColor[0]);

	fileBuffer += sizeof(difuseColor);

	/*float3 specularColor[1];
	memcpy(specularColor, fileBuffer, sizeof(specularColor));
	resource->SetSpecularColor(specularColor[0]);

	fileBuffer += sizeof(specularColor);*/

	/*float* shininess = new float;
	memcpy(shininess, fileBuffer, sizeof(float));
	resource->SetShininess(*shininess);*/

	//fileBuffer += sizeof(float);

	float* normalStrenght = new float;
	memcpy(normalStrenght, fileBuffer, sizeof(float));
	resource->SetNormalStrength(*normalStrenght);

	fileBuffer += sizeof(float);

	bool* isTransparent = new bool;
	memcpy(isTransparent, fileBuffer, sizeof(bool));
	resource->SetTransparent(*isTransparent);

	fileBuffer += sizeof(bool);

	//delete shininess;
	delete normalStrenght;
	delete isTransparent;

#ifdef ENGINE
	resource->LoadLoadOptions(meta);
#endif // ENGINE

}
