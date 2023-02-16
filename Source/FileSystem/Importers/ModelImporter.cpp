#pragma warning (disable: 26495)
#pragma warning (disable: 6386)

#include "ModelImporter.h"

#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"

#include "Math/float3.h"

#include "assimp/scene.h"     
#include "assimp/postprocess.h"
#include "assimp/cimport.h"

void myCallback(const char* msg, char* userData) {
	ENGINE_LOG("[assimp] %s", msg);
}

ModelImporter::ModelImporter()
{
}

ModelImporter::~ModelImporter()
{
}

void ModelImporter::Import(const char* filePath, std::shared_ptr<ResourceModel> resource)
{
	struct aiLogStream stream;
	stream.callback = myCallback;
	aiAttachLogStream(&stream);

	ENGINE_LOG("Import Model from %s", filePath);

	const aiScene* scene = aiImportFile(filePath, aiProcess_Triangulate);
	if (scene)
	{
		ImportMaterials(scene, filePath, resource);
		ImportMeshes(scene, filePath, resource);
		aiReleaseImport(scene);

		char* buffer{};
		unsigned int size;
		Save(resource, buffer, size);
		App->fileSystem->Save((resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str() , buffer, size);

		delete buffer;
	}
	else
	{
		ENGINE_LOG("Error loading %s: %s", filePath, aiGetErrorString());
	}
}

void ModelImporter::Save(const std::shared_ptr<ResourceModel>& resource, char*& fileBuffer, unsigned int& size)
{
	unsigned int header[2] = { (unsigned int)resource->GetNumMeshes(), (unsigned int)resource->GetNumMaterials() };

	size = sizeof(header) + sizeof(UID) * (unsigned int)resource->GetNumMeshes() + sizeof(UID) * (unsigned int)resource->GetNumMaterials();

	char* cursor = new char[size]{};

	fileBuffer = cursor;

	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

	bytes = sizeof(UID) * (unsigned int)resource->GetNumMeshes();
	memcpy(cursor, &(resource->GetMeshesUIDs()[0]), bytes);

	cursor += bytes;

	bytes = sizeof(UID) * (unsigned int)resource->GetNumMaterials();
	memcpy(cursor, &(resource->GetMaterialsUIDs()[0]), bytes);
}

void ModelImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceModel> resource)
{
	unsigned int header[2];
	unsigned int bytes = sizeof(header);
	memcpy(header, fileBuffer, bytes);

	resource->SetNumMeshes(header[0]);
	resource->SetNumMaterials(header[1]);

	fileBuffer += bytes;

	UID* meshesPointer = new UID[resource->GetNumMeshes()];
	bytes = sizeof(UID) * (unsigned int)resource->GetNumMeshes();
	memcpy(meshesPointer, fileBuffer, bytes);
	std::vector<UID> meshes(meshesPointer, meshesPointer + resource->GetNumMeshes());
	resource->SetMeshesUIDs(meshes);

	fileBuffer += bytes;

	delete[] meshesPointer;

	UID* materialsPointer = new UID[resource->GetNumMaterials()];
	bytes = sizeof(UID) * (unsigned int)resource->GetNumMaterials();
	memcpy(materialsPointer, fileBuffer, bytes);
	std::vector<UID> materials(materialsPointer, materialsPointer + resource->GetNumMaterials());
	resource->SetMaterialsUIDs(materials);

	delete[] materialsPointer;
}


void ModelImporter::ImportMaterials(const aiScene* scene, const char* filePath, std::shared_ptr<ResourceModel>& resource)
{
	ENGINE_LOG("---- Loading Materials ----");

	std::vector<UID> materialsUIDs;
	materialsUIDs.reserve(scene->mNumMaterials);

	for (unsigned i = 0; i < scene->mNumMaterials; ++i)
	{
		aiString file;

		aiMaterial* material = scene->mMaterials[i];

		std::vector<std::string> pathTextures(4);

		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &file) == AI_SUCCESS)
		{
			std::string diffusePath = "";

			CheckPathMaterial(filePath, file, diffusePath);

			if (diffusePath != "")
			{
				pathTextures[0] = diffusePath;
			}
		}
		//Getting the specular texture
		if (material->GetTexture(aiTextureType_SPECULAR, 0, &file) == AI_SUCCESS)
		{
			std::string specularPath = "";

			CheckPathMaterial(filePath, file, specularPath);

			if (specularPath != "")
			{
				pathTextures[1] = specularPath;
			}
		}

		if (scene->mMaterials[i]->GetTexture(aiTextureType_NORMALS, 0, &file) == AI_SUCCESS)
		{
			std::string normalPath = "";

			CheckPathMaterial(filePath, file, normalPath);

			if (normalPath != "")
			{
				pathTextures[1] = normalPath;
			}
		}

		char* fileBuffer{};
		unsigned int size = 0;

		App->fileSystem->SaveInfoMaterial(pathTextures, fileBuffer, size);
		std::string materialPath = MATERIAL_PATH + resource->GetFileName() + "_" + std::to_string(i) + MATERIAL_EXTENSION;

		App->fileSystem->Save(materialPath.c_str(), fileBuffer, size);
		UID resourceMaterial = App->resources->ImportResource(materialPath);
		materialsUIDs.push_back(resourceMaterial);

		delete fileBuffer;
	}

	resource->SetMaterialsUIDs(materialsUIDs);
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
		std::string meshPath = MESHES_PATH + resource->GetFileName() + "_" + std::to_string(i) + MESH_EXTENSION;

		App->fileSystem->Save(meshPath.c_str(),fileBuffer,size);
		UID resourceMesh = App->resources->ImportResource(meshPath);
		meshesUIDs.push_back(resourceMesh);
	}
	resource->SetMeshesUIDs(meshesUIDs);
}

void ModelImporter::CheckPathMaterial(const char* filePath, const aiString& file, std::string& dataBuffer)
{
	struct stat buffer {};
	std::string name = App->fileSystem->GetFileName(file.data);
	name += App->fileSystem->GetFileExtension(file.data);
	
	// Cheking by name
	if (stat(file.data, &buffer) != 0)
	{
		std::string path = App->fileSystem->GetPathWithoutFile(filePath);
		//Checking in the original fbx folder
		if (stat((path + name).c_str(), &buffer) != 0)
		{
			// Cheking in asset textures folder
			if (stat((TEXTURES_PATH + name).c_str(), &buffer) != 0)
			{
				ENGINE_LOG("Texture not found!");
			}
			else
				dataBuffer = TEXTURES_PATH + name;
		}
		else
			dataBuffer = path + name;
	}
	else
		dataBuffer = std::string(file.data);

}

void ModelImporter::SaveInfoMesh(const aiMesh* ourMesh, char*& fileBuffer, unsigned int &size) {

	unsigned int numIndexes = 3;

	unsigned int hasTangents = ourMesh->mTangents != nullptr;

	unsigned int header[4] = 
	{ 
		ourMesh->mNumFaces, 
		ourMesh->mNumVertices, 
		ourMesh->mMaterialIndex,
		hasTangents
	};

	unsigned int sizeOfVectors = sizeof(float3) * ourMesh->mNumVertices;
	unsigned int numOfVectors = 3;
	if (hasTangents)
	{
		numOfVectors = 4;
	}
	size = sizeof(header) + ourMesh->mNumFaces * (sizeof(unsigned int) * numIndexes)
		+ static_cast<unsigned long long>(sizeOfVectors) * static_cast<unsigned long long>(numOfVectors);
	
	char* cursor = new char[size] {};

	fileBuffer = cursor;

	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

	if (ourMesh->mVertices != nullptr)
	{
		bytes = sizeof(float3) * ourMesh->mNumVertices;
		memcpy(cursor, &(ourMesh->mVertices[0]), bytes);

		cursor += bytes;
	}

	if (ourMesh->mTextureCoords != nullptr)
	{
		bytes = sizeof(float3) * ourMesh->mNumVertices;
		memcpy(cursor, &(ourMesh->mTextureCoords[0][0]), bytes);

		cursor += bytes;
	}

	if (ourMesh->mNormals != nullptr)
	{
		bytes = sizeof(float3) * ourMesh->mNumVertices;
		memcpy(cursor, &(ourMesh->mNormals[0]), bytes);

		cursor += bytes;
	}

	if (hasTangents) 
	{
		bytes = sizeof(float3) * ourMesh->mNumVertices;
		memcpy(cursor, &(ourMesh->mTangents[0]), bytes);

		cursor += bytes;
	}

	for (unsigned int i = 0; i < ourMesh->mNumFaces; ++i) 
	{
		bytes = sizeof(unsigned int) * numIndexes;
		memcpy(cursor, &(ourMesh->mFaces[i].mIndices[0]), bytes);

		cursor += bytes;
	}
}

