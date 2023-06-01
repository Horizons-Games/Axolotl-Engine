#include "MaterialImporter.h"
#include "Application.h"
#include "DataModels/Resources/ResourceTexture.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"

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

	fileSystem->Load(filePath, bufferPaths);

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
			resourceTexture.push_back(
				std::dynamic_pointer_cast<ResourceTexture>(App->GetModule<ModuleResources>()->ImportResource(path)));
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
		switch (resource->GetShaderType())
		{
			case 0:

				resource->SetMetallic(resourceTexture[3]);

				break;

			case 1:

				resource->SetSpecular(resourceTexture[3]);

				break;
		}
	}

	char* buffer{};
	unsigned int size;

	Save(resource, buffer, size);
	fileSystem->Save((resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), buffer, size);

	delete buffer;
}

void MaterialImporter::Save(const std::shared_ptr<ResourceMaterial>& resource, char*& fileBuffer, unsigned int& size)
{
#ifdef ENGINE

	std::string metaPath = resource->GetAssetsPath() + META_EXTENSION;
	char* metaBuffer = {};
	rapidjson::Document doc;
	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();

	fileSystem->Load(metaPath.c_str(), metaBuffer);
	Json meta(doc, doc);
	meta.fromBuffer(metaBuffer);

	delete metaBuffer;

	if (resource->GetDiffuse())
	{
		meta["DiffuseAssetPath"] = resource->GetDiffuse()->GetAssetsPath().c_str();
	}
	else
	{
		meta["DiffuseAssetPath"] = "";
	}

	if (resource->GetNormal())
	{
		meta["NormalAssetPath"] = resource->GetNormal()->GetAssetsPath().c_str();
	}
	else
	{
		meta["NormalAssetPath"] = "";
	}

	if (resource->GetOcclusion())
	{
		meta["OcclusionAssetPath"] = resource->GetOcclusion()->GetAssetsPath().c_str();
	}
	else
	{
		meta["OcclusionAssetPath"] = "";
	}

	switch (resource->GetShaderType())
	{
		case 0:

			if (resource->GetMetallic())
			{
				meta["SpecularAssetPath"] = resource->GetMetallic()->GetAssetsPath().c_str();
			}
			else
			{
				meta["SpecularAssetPath"] = "";
			}

			break;
		case 1:

			if (resource->GetSpecular())
			{
				meta["SpecularAssetPath"] = resource->GetSpecular()->GetAssetsPath().c_str();
			}
			else
			{
				meta["SpecularAssetPath"] = "";
			}

			break;
	}

	rapidjson::StringBuffer buffer;

	meta.toBuffer(buffer);
	fileSystem->Save(metaPath.c_str(), buffer.GetString(), static_cast<unsigned int>(buffer.GetSize()));

#endif

	UID specularUID;

	switch (resource->GetShaderType())
	{
		case 0:

			if (resource->GetMetallic())
			{
				specularUID = resource->GetMetallic()->GetUID();
			}
			else
			{
				specularUID = 0;
			}

			break;
		case 1:

			if (resource->GetSpecular())
			{
				specularUID = resource->GetSpecular()->GetUID();
			}
			else
			{
				specularUID = 0;
			}

			break;
	}

	UID texturesUIDs[4] = { resource->GetDiffuse() ? resource->GetDiffuse()->GetUID() : 0,
							resource->GetNormal() ? resource->GetNormal()->GetUID() : 0,
							resource->GetOcclusion() ? resource->GetOcclusion()->GetUID() : 0,
							specularUID };

	float4 diffuseColor[1] = { resource->GetDiffuseColor() };
	float3 specularColor[1] = { resource->GetSpecularColor() };

	size = sizeof(texturesUIDs) + sizeof(diffuseColor) + sizeof(specularColor) + sizeof(float) * 3 + sizeof(bool) +
		   sizeof(unsigned int);

	char* cursor = new char[size];

	fileBuffer = cursor;

	unsigned int bytes = sizeof(texturesUIDs);
	memcpy(cursor, texturesUIDs, bytes);

	cursor += bytes;

	bytes = sizeof(unsigned int);
	memcpy(cursor, &resource->GetShaderType(), bytes);

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
	memcpy(cursor, &resource->IsTransparent(), bytes);

	cursor += bytes;

	bytes = sizeof(float);
	memcpy(cursor, &resource->GetSmoothness(), bytes);

	cursor += bytes;

	bytes = sizeof(float);
	memcpy(cursor, &resource->GetMetalness(), bytes);
}

void MaterialImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceMaterial> resource)
{
	UID texturesUIDs[4];
	memcpy(texturesUIDs, fileBuffer, sizeof(texturesUIDs));

	fileBuffer += sizeof(texturesUIDs);

	unsigned int* shaderType = new unsigned int;

	memcpy(shaderType, fileBuffer, sizeof(unsigned int));
	resource->SetShaderType(*shaderType);

	fileBuffer += sizeof(unsigned int);

#ifdef ENGINE

	std::string metaPath = resource->GetAssetsPath() + META_EXTENSION;
	char* metaBuffer = {};
	rapidjson::Document doc;
	ModuleResources* resources = App->GetModule<ModuleResources>();
	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();

	fileSystem->Load(metaPath.c_str(), metaBuffer);
	Json meta(doc, doc);
	meta.fromBuffer(metaBuffer);

	delete metaBuffer;

	std::string assetPath = meta["DiffuseAssetPath"];

	if (assetPath != "")
	{
		bool materialExists = assetPath != "" && fileSystem->Exists(assetPath.c_str());
		if (materialExists)
		{
			resource->SetDiffuse(resources->RequestResource<ResourceTexture>(assetPath));
		}
	}

	assetPath = meta["NormalAssetPath"];

	if (assetPath != "")
	{
		bool materialExists = assetPath != "" && fileSystem->Exists(assetPath.c_str());
		if (materialExists)
		{
			resource->SetNormal(resources->RequestResource<ResourceTexture>(assetPath));
		}
	}

	assetPath = meta["OcclusionAssetPath"];

	if (assetPath != "")
	{
		bool materialExists = assetPath != "" && fileSystem->Exists(assetPath.c_str());
		if (materialExists)
		{
			resource->SetOcclusion(resources->RequestResource<ResourceTexture>(assetPath));
		}
	}

	assetPath = meta["SpecularAssetPath"];

	if (assetPath != "")
	{
		switch (*shaderType)
		{
			case 0:

				if (assetPath != "" && fileSystem->Exists(assetPath.c_str()))
				{
					resource->SetMetallic(resources->RequestResource<ResourceTexture>(assetPath));
				}

				break;

			case 1:

				if (assetPath != "" && fileSystem->Exists(assetPath.c_str()))
				{
					resource->SetSpecular(resources->RequestResource<ResourceTexture>(assetPath));
				}

				break;
		}
	}

#else

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

#endif

	float4 diffuseColor;

	memcpy(&diffuseColor, fileBuffer, sizeof(diffuseColor));
	resource->SetDiffuseColor(diffuseColor);

	fileBuffer += sizeof(diffuseColor);

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

	float smoothness;
	memcpy(&smoothness, fileBuffer, sizeof(float));
	resource->SetSmoothness(smoothness);

	fileBuffer += sizeof(float);

	float metalness;
	memcpy(&metalness, fileBuffer, sizeof(float));
	resource->SetMetalness(metalness);

	fileBuffer += sizeof(float);

	delete shaderType;
	delete normalStrenght;
	delete isTransparent;

#ifdef ENGINE

	resource->LoadLoadOptions(meta);

#endif
}
