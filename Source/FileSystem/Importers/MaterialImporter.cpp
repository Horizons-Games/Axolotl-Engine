#include "MaterialImporter.h"
#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"


void MaterialImporter::Import(const char* filePath, std::shared_ptr<ResourceMaterial> resource)
{
	char* bufferPaths;

	App->fileSystem->Load(filePath, bufferPaths);

	unsigned int header[4];
	memcpy(header, bufferPaths, sizeof(header));

	bufferPaths += sizeof(header);

	std::vector<std::shared_ptr<Resource>> resourceTexture;

	for (int i = 0; i < 4; ++i)
	{
		char* pathPointer = new char[header[i]];
		memcpy(pathPointer, bufferPaths, header[i]);
		std::string path(pathPointer, pathPointer + header[i]);

		if (!path.empty()) 
		{
			resourceTexture.push_back(App->resources->ImportResource(path));
		}
		else 
		{
			resourceTexture.push_back(0);
		}

		bufferPaths += header[i];

		delete[] pathPointer;
	}

	if(resourceTexture[0]) resource->SetDiffuse(resourceTexture[0]);
	if(resourceTexture[1]) resource->SetNormal(resourceTexture[1]);
	if(resourceTexture[2]) resource->SetOcclusion(resourceTexture[2]);
	if(resourceTexture[3]) resource->SetSpecular(resourceTexture[3]);

	char* buffer{};
	unsigned int size;
	Save(resource, buffer, size);
	App->fileSystem->Save((resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), buffer, size);

	delete buffer;
}

void MaterialImporter::Save(const std::shared_ptr<ResourceMaterial>& resource, char*& fileBuffer, unsigned int& size)
{
    UID texturesUIDs[4] = 
	{ 
		resource->GetDiffuse()->GetUID(),
		resource->GetNormal()->GetUID(),
		resource->GetOcclusionr()->GetUID(),
		resource->GetSpecular()->GetUID()
	};

	float3 colors[2] =
	{
		resource->GetDiffuseColor(),
		resource->GetSpecularColor()
	};

	size = sizeof(texturesUIDs) + sizeof(colors) + sizeof(float) * 2;

	char* cursor = new char[size];

	fileBuffer = cursor;

	unsigned int bytes = sizeof(texturesUIDs);
	memcpy(cursor, texturesUIDs, bytes);

	cursor += bytes;

	bytes = sizeof(colors);
	memcpy(cursor, colors, bytes);

	cursor += bytes;

	bytes = sizeof(float);
	memcpy(cursor, &resource->GetShininess(), bytes);

	cursor += bytes;

	bytes = sizeof(float);
	memcpy(cursor, &resource->GetNormalStrength(), bytes);
}

void MaterialImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceMaterial> resource)
{
	UID texturesUIDs[4];
	memcpy(texturesUIDs, fileBuffer, sizeof(texturesUIDs));



	/*if(texturesUIDs[0] != 0) resource->SetDiffuseUID(texturesUIDs[0]);
	if(texturesUIDs[1] != 0) resource->SetNormalUID(texturesUIDs[1]);
	if(texturesUIDs[2] != 0) resource->SetOcclusionUID(texturesUIDs[2]);
	if(texturesUIDs[3] != 0) resource->SetSpecularUID(texturesUIDs[3]);*/

	fileBuffer += sizeof(texturesUIDs);

	float3 colors[2];
	memcpy(colors, fileBuffer, sizeof(colors));

	resource->SetDiffuseColor(colors[0]);
	resource->SetSpecularColor(colors[1]);

	fileBuffer += sizeof(colors);

	float* shininess = new float;
	memcpy(shininess, fileBuffer, sizeof(float));
	resource->SetShininess(*shininess);

	fileBuffer += sizeof(float);

	float* normalStrenght = new float;
	memcpy(normalStrenght, fileBuffer, sizeof(float));
	resource->SetNormalStrength(*normalStrenght);

	delete shininess;
	delete normalStrenght;
}
