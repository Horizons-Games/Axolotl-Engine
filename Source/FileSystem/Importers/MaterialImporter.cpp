#include "MaterialImporter.h"
#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"
#include "DataModels/Resources/ResourceTexture.h"

MaterialImporter::MaterialImporter()
{
}

MaterialImporter::~MaterialImporter()
{
}

void MaterialImporter::Import
	(const char* filePath, std::shared_ptr<ResourceMaterial> resource)
{
	char* bufferPaths;

	App->fileSystem->Load(filePath, bufferPaths);

	unsigned int header[5];
	memcpy(header, bufferPaths, sizeof(header));

	bufferPaths += sizeof(header);

	std::vector<std::shared_ptr<ResourceTexture>> resourceTexture;

	for (int i = 0; i < 5; ++i)
	{
		char* pathPointer = new char[header[i]];
		memcpy(pathPointer, bufferPaths, header[i]);
		std::string path(pathPointer, pathPointer + header[i]);

		if (!path.empty()) 
		{
			resourceTexture.push_back
			(std::dynamic_pointer_cast<ResourceTexture>
				(App->resources->ImportResource(path)));
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
	
	if (resourceTexture[3] != 0)
	{
		resource->SetMetallic(resourceTexture[3]);
	}

	if (resourceTexture[4] != 0)
	{
		resource->SetSpecular(resourceTexture[4]);
	}

	char* buffer{};
	unsigned int size;

	Save(resource, buffer, size);
	App->fileSystem->Save
		((resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(),
			buffer, size);

	delete buffer;
}

void MaterialImporter::Save
	(const std::shared_ptr<ResourceMaterial>& resource, 
		char*& fileBuffer, unsigned int& size)
{
#ifdef ENGINE

	std::string metaPath = resource->GetAssetsPath() + META_EXTENSION;
	char* metaBuffer = {};
	rapidjson::Document doc;

	App->fileSystem->Load(metaPath.c_str(), metaBuffer);
	Json meta(doc, doc);
	meta.fromBuffer(metaBuffer);

	delete metaBuffer;

	if (resource->GetDiffuse())
	{
		meta["DiffuseAssetPath"] = 
			resource->GetDiffuse()->GetAssetsPath().c_str();
	}
	else
	{
		meta["DiffuseAssetPath"] = "";
	}

	if (resource->GetNormal())
	{
		meta["NormalAssetPath"] = 
			resource->GetNormal()->GetAssetsPath().c_str();
	}
	else
	{
		meta["NormalAssetPath"] = "";
	}

	if (resource->GetOcclusion())
	{
		meta["OcclusionAssetPath"] = 
			resource->GetOcclusion()->GetAssetsPath().c_str();
	}
	else
	{
		meta["OcclusionAssetPath"] = "";
	}

	if (resource->GetMetallic())
	{
		meta["MetallicAssetPath"] = 
			resource->GetMetallic()->GetAssetsPath().c_str();
	}
	else
	{
		meta["MetallicAssetPath"] = "";
	}

	if (resource->GetSpecular())
	{
		meta["SpecularAssetPath"] = 
			resource->GetSpecular()->GetAssetsPath().c_str();
	}
	else
	{
		meta["SpecularAssetPath"] = "";
	}

	rapidjson::StringBuffer buffer;

	meta.toBuffer(buffer);
	App->fileSystem->
		Save(metaPath.c_str(), buffer.GetString(), 
			static_cast<unsigned int>(buffer.GetSize()));

#endif

	UID texturesUIDs[5] =
	{
		resource->GetDiffuse() ? resource->GetDiffuse()->GetUID() : 0,
		resource->GetNormal() ? resource->GetNormal()->GetUID() : 0,
		resource->GetOcclusion() ? resource->GetOcclusion()->GetUID() : 0,
		resource->GetSpecular() ? resource->GetSpecular()->GetUID() : 0,
		resource->GetMetallic() ? resource->GetMetallic()->GetUID() : 0
	};

	float4 diffuseColor[1] = { resource->GetDiffuseColor() };
	float3 specularColor[1] = { resource->GetSpecularColor() };
	
	size = sizeof(texturesUIDs) + sizeof(diffuseColor) 
		+ sizeof(specularColor)  + sizeof(float) * 2 + sizeof(bool);

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

	bytes = sizeof(float);
	memcpy(cursor, &resource->GetNormalStrength(), bytes);

	cursor += bytes;

	bytes = sizeof(bool);
	memcpy(cursor, &resource->GetTransparent(), bytes);

	cursor += bytes;

	bytes = sizeof(unsigned int);
	memcpy(cursor, &resource->GetShaderType(), bytes);
}

void MaterialImporter::Load
	(const char* fileBuffer, std::shared_ptr<ResourceMaterial> resource)
{
	UID texturesUIDs[5];
	memcpy(texturesUIDs, fileBuffer, sizeof(texturesUIDs));

#ifdef ENGINE

	std::string metaPath = resource->GetAssetsPath() + META_EXTENSION;
	char* metaBuffer = {};
	rapidjson::Document doc;

	App->fileSystem->Load(metaPath.c_str(), metaBuffer);
	Json meta(doc, doc);
	meta.fromBuffer(metaBuffer);

	delete metaBuffer;

	std::string assetPath = meta["DiffuseAssetPath"];

	if (assetPath != "") 
	{ 
		resource->SetDiffuse(App->resources->RequestResource<ResourceTexture>(assetPath));
	}

	assetPath = meta["NormalAssetPath"];

	if (assetPath != "")
	{
		resource->SetNormal(App->resources->RequestResource<ResourceTexture>(assetPath));
	}

	assetPath = meta["OcclusionAssetPath"];

	if (assetPath != "")
	{ 
		resource->SetOcclusion(App->resources->RequestResource<ResourceTexture>(assetPath));
	}

	assetPath = meta["SpecularAssetPath"];

	if (assetPath != "")
	{ 
		resource->SetSpecular(App->resources->RequestResource<ResourceTexture>(assetPath));
	}

	assetPath = meta["MetallicAssetPath"];

	if (assetPath != "")
	{ 
		resource->SetMetallic(App->resources->RequestResource<ResourceTexture>(assetPath));
	}

#else
	
	if (texturesUIDs[0] != 0)
	{
		resource->SetDiffuse
			(App->resources->SearchResource<ResourceTexture>
												(texturesUIDs[0]));
	}

	if (texturesUIDs[1] != 0)
	{
		resource->SetNormal
			(App->resources->SearchResource<ResourceTexture>
												(texturesUIDs[1]));
	}
		
	if (texturesUIDs[2] != 0)
	{
		resource->SetOcclusion
			(App->resources->SearchResource<ResourceTexture>
												(texturesUIDs[2]));
	}
		
	if (texturesUIDs[3] != 0)
	{
		resource->SetMetallic
			(App->resources->SearchResource<ResourceTexture>
												(texturesUIDs[3]));
	}

	if (texturesUIDs[4] != 0)
	{
		resource->SetSpecular
			(App->resources->SearchResource<ResourceTexture>
												(texturesUIDs[4]));
	}

#endif

	fileBuffer += sizeof(texturesUIDs);

	float4 difuseColor;

	memcpy(&difuseColor, fileBuffer, sizeof(difuseColor));
	resource->SetDiffuseColor(difuseColor);

	fileBuffer += sizeof(difuseColor);

	float3 specularColor;

	memcpy(&specularColor, fileBuffer, sizeof(specularColor));
	resource->SetSpecularColor(specularColor);

	fileBuffer += sizeof(specularColor);

	float* normalStrenght = new float;

	memcpy(normalStrenght, fileBuffer, sizeof(float));
	resource->SetNormalStrength(*normalStrenght);

	fileBuffer += sizeof(float);

	bool* isTransparent = new bool;

	memcpy(isTransparent, fileBuffer, sizeof(bool));
	resource->SetTransparent(*isTransparent);

	fileBuffer += sizeof(bool);

	unsigned int* shaderType = new unsigned int;

	memcpy(shaderType, fileBuffer, sizeof(unsigned int));
	resource->SetShaderType(*shaderType);

	fileBuffer += sizeof(unsigned int);

	delete normalStrenght;
	delete isTransparent;
	delete shaderType;

#ifdef ENGINE

	resource->LoadLoadOptions(meta);

#endif

}
