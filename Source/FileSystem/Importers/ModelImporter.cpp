#include "ModelImporter.h"

#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"
#include "EngineLog.h"

#include "Math/float3.h"

#include "assimp/scene.h"     
#include "assimp/postprocess.h"
#include "assimp/cimport.h"
#include "assimp/types.h"
#include "assimp/material.h"

void ModelImporter::Import(const char* filePath, std::shared_ptr<ResourceModel> resource)
{
	ENGINE_LOG("Import Model from %s", filePath);

	const aiScene* scene = aiImportFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene)
	{
		ImportMaterials(scene, filePath, resource);
		ImportMeshes(scene, filePath, resource);
	}
	else
	{
		ENGINE_LOG("Error loading %s: %s", filePath, aiGetErrorString());
	}
}

uint64_t ModelImporter::Save(std::shared_ptr<ResourceModel> resource, char*& fileBuffer)
{
	// Provisional return, here we have to return serialize UID for the object
	return 0;
}

void ModelImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceModel> resource)
{
}


void ModelImporter::ImportMaterials(const aiScene* scene, const char* filePath, std::shared_ptr<ResourceModel>& resource)
{
	ENGINE_LOG("---- Loading textures ----");

	aiString file;

	std::vector<UID> textureUIDs;
	textureUIDs.reserve(scene->mNumMaterials);

	for (unsigned i = 0; i < scene->mNumMaterials; ++i)
	{
		if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &file) == AI_SUCCESS)
		{
			std::string texturePath = TEXTURES_PATH + std::string(file.data);

			struct stat buffer {};
			// Cheking by name
			if (stat(file.data, &buffer) != 0)
			{
				//Checking in the fbx folder
				if (stat((std::string(filePath) + std::string(file.data)).c_str(), &buffer) != 0)
				{
					// Cheking in textures folder
					if (stat((TEXTURES_PATH + std::string(file.data)).c_str(), &buffer) != 0)
					{
						ENGINE_LOG("Texture not found!");
					}
					else
						texturePath = TEXTURES_PATH + std::string(file.data);
				}
				else
					texturePath = filePath + std::string(file.data);
			}
			else
				texturePath = std::string(file.data);

			UID resourceTexture = App->resources->ImportResource(texturePath);
			textureUIDs.push_back(resourceTexture);
		}
	}

	resource->SetTexturesUIDs(textureUIDs);
}

void ModelImporter::ImportMeshes(const aiScene* scene, const char* filePath, std::shared_ptr<ResourceModel>& resource)
{
	std::vector<UID> meshesUIDs;
	meshesUIDs.reserve(scene->mNumMeshes);

	for (unsigned i = 0; i < scene->mNumMeshes; ++i)
	{
		char* fileBuffer{};
		aiMesh* ourMesh = scene->mMeshes[i];
		unsigned int size = 0;

		SaveInfoMesh(ourMesh, fileBuffer, size);
		std::string meshPath = MESHES_LIB_PATH + resource->GetFileName() + "_" + std::to_string(i) + MESH_EXTENSION;

		App->fileSystem->Save(meshPath.c_str(),fileBuffer,size);
		UID resourceMesh = App->resources->ImportResource(meshPath);
		meshesUIDs.push_back(resourceMesh);

		App->fileSystem->Delete(meshPath.c_str());
	}
	resource->SetMeshesUIDs(meshesUIDs);
}

void ModelImporter::SaveInfoMesh(const aiMesh* ourMesh, char*& fileBuffer, unsigned int &size) {

	unsigned int numIndexes = 3;

	unsigned int header[3] = { ourMesh->mNumFaces, ourMesh->mNumVertices, ourMesh->mMaterialIndex };

	size = sizeof(header) + ourMesh->mNumFaces * (sizeof(unsigned int) * numIndexes)
		+ sizeof(float3) * ourMesh->mNumVertices * 2;

	char* cursor = new char[size] {};

	fileBuffer = cursor;

	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

	bytes = ourMesh->mNumFaces * (sizeof(unsigned int) * numIndexes);
	memcpy(cursor, &ourMesh->mFaces[0].mIndices[0], bytes);

	cursor += bytes;

	bytes = sizeof(float3) * ourMesh->mNumVertices;
	memcpy(cursor, &ourMesh->mVertices, bytes);

	cursor += bytes;

	bytes = sizeof(float3) * ourMesh->mNumVertices;
	memcpy(cursor, &ourMesh->mTextureCoords, bytes);

}
