#pragma warning (disable: 26495)
#pragma warning (disable: 6386)

#include "ModelImporter.h"

#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"
#include "DataModels/Resources/ResourceMesh.h"
#include "DataModels/Resources/ResourceMaterial.h"

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

	const aiScene* scene = aiImportFile(filePath, aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	if (scene)
	{
		ImportNode(scene, filePath, resource, scene->mRootNode, -1);
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
#ifdef ENGINE
	//Update Meta
	std::string metaPath = resource->GetAssetsPath() + META_EXTENSION;
	char* metaBuffer = {};
	App->fileSystem->Load(metaPath.c_str(), metaBuffer);
	rapidjson::Document doc;
	Json meta(doc, doc);
	meta.fromBuffer(metaBuffer);
	delete metaBuffer;
#endif
	
	size = (sizeof(float4x4) + sizeof(int) + sizeof(unsigned int) * 2) * resource->GetNumNodes();

	for (ResourceModel::Node *node : resource->GetNodes())
	{
		size += sizeof(UID) * 2 * node->meshRenderers.size();
		size += sizeof(char) * node->name.size();
	}

	unsigned int header[1] = { (unsigned int)resource->GetNumNodes() };
	size += sizeof(header);

	char* cursor = new char[size] {};

	fileBuffer = cursor;

	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

#ifdef ENGINE
	Json jsonMeshes = meta["MeshesAssetPaths"];
	Json jsonMat = meta["MatAssetPaths"];
	unsigned int countMeshes = 0;
	unsigned int countMat = 0;
#endif

	for (ResourceModel::Node* node : resource->GetNodes())
	{
		unsigned int nodeHeader[2] = { node->name.size(), node->meshRenderers.size() };

		bytes = sizeof(nodeHeader);
		memcpy(cursor, nodeHeader, bytes);
		cursor += bytes;

		bytes = sizeof(char) * node->name.size();
		memcpy(cursor, &(node->name[0]), bytes);
		cursor += bytes;

		bytes = sizeof(float4x4);
		memcpy(cursor, &(node->transform[0]), bytes);
		cursor += bytes;

		bytes = sizeof(int);
		memcpy(cursor, &(node->parent), bytes);
		cursor += bytes;

		std::vector<UID> meshesUIDs;
		meshesUIDs.reserve(node->meshRenderers.size());
		for (int i = 0; i < node->meshRenderers.size(); i++)
		{
#ifdef ENGINE
			jsonMeshes[countMeshes] = node->meshRenderers[i].first->GetAssetsPath().c_str();

			++countMeshes;
#endif

			meshesUIDs.push_back(node->meshRenderers[i].first->GetUID());

			bytes = sizeof(UID);
			memcpy(cursor, &(meshesUIDs[0]), bytes);

			cursor += bytes;
		}

		std::vector<UID> materialsUIDs;
		materialsUIDs.reserve(node->meshRenderers.size());
		for (int i = 0; i < node->meshRenderers.size(); i++)
		{
#ifdef ENGINE
			jsonMat[countMat] = node->meshRenderers[i].second->GetAssetsPath().c_str();

			++countMat;
#endif
			materialsUIDs.push_back(node->meshRenderers[i].second->GetUID());

			bytes = sizeof(UID);
			memcpy(cursor, &(materialsUIDs[0]), bytes);

			cursor += bytes;
		}
	}

#ifdef ENGINE
	rapidjson::StringBuffer buffer;
	meta.toBuffer(buffer);
	App->fileSystem->Save(metaPath.c_str(), buffer.GetString(), (unsigned int)buffer.GetSize());
#endif
}

void ModelImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceModel> resource)
{
#ifdef ENGINE
	//Update Meta
	std::string metaPath = resource->GetAssetsPath() + META_EXTENSION;
	char* metaBuffer = {};
	App->fileSystem->Load(metaPath.c_str(), metaBuffer);
	rapidjson::Document doc;
	Json meta(doc, doc);
	meta.fromBuffer(metaBuffer);
	delete metaBuffer;
#endif

	unsigned int header[1];
	unsigned int bytes = sizeof(header);
	memcpy(header, fileBuffer, bytes);
	fileBuffer += bytes;

	std::vector<ResourceModel::Node*> nodes;
	nodes.reserve(header[0]);

#ifdef ENGINE
	Json jsonMeshes = meta["MeshesAssetPaths"];
	Json jsonMat = meta["MatAssetPaths"];
	unsigned int countMeshes = 0;
	unsigned int countMat = 0;
#endif

	for(unsigned int i = 0; i < header[0]; ++i)
	{
		ResourceModel::Node* node = new ResourceModel::Node;

		unsigned int nodeHeader[2];
		bytes = sizeof(nodeHeader);
		memcpy(nodeHeader, fileBuffer, bytes);
		fileBuffer += bytes;

		char* name = new char[nodeHeader[0]] {};
		bytes = sizeof(char) * (unsigned int)nodeHeader[0];
		memcpy(name, fileBuffer, bytes);
		fileBuffer += bytes;
		node->name = std::string(name, nodeHeader[0]);
		delete name;

		float4x4* transform = new float4x4;
		bytes = sizeof(float4x4);
		memcpy(transform, fileBuffer, bytes);
		fileBuffer += bytes;
		node->transform = *transform;
		delete transform;

		int* parent = new int;
		bytes = sizeof(int);
		memcpy(parent, fileBuffer, bytes);
		fileBuffer += bytes;
		node->parent = *parent;
		delete parent;

		node->meshRenderers.reserve(nodeHeader[1]);

#ifdef  ENGINE
		for (int i = 0; i < nodeHeader[1]; i++)
		{
			std::string meshPath = jsonMeshes[countMeshes];
			std::string matPath = jsonMat[countMat];

			std::shared_ptr<ResourceMesh> mesh = App->resources->RequestResource<ResourceMesh>(meshPath);
			std::shared_ptr<ResourceMaterial> material = App->resources->RequestResource<ResourceMaterial>(matPath);

			node->meshRenderers.push_back(std::make_pair(mesh, material));

			++countMeshes;
			++countMat;

			fileBuffer += sizeof(UID) * 2;
		}


		nodes.push_back(node);
#else
		UID* meshesPointer = new UID[nodeHeader[1]];
		bytes = sizeof(UID) * (unsigned int)nodeHeader[1];
		memcpy(meshesPointer, fileBuffer, bytes);
		std::vector<UID> meshesUIDs(meshesPointer, meshesPointer + nodeHeader[1]);
		delete[] meshesPointer;
		fileBuffer += bytes;

		UID* materialsPointer = new UID[nodeHeader[1]];
		bytes = sizeof(UID) * (unsigned int)nodeHeader[1];
		memcpy(materialsPointer, fileBuffer, bytes);
		std::vector<UID> materialsUIDs(materialsPointer, materialsPointer + nodeHeader[1]);
		delete[] materialsPointer;
		fileBuffer += bytes;

		for (int i = 0; i < meshesUIDs.size(); i++)
		{
			std::shared_ptr<ResourceMesh> mesh = App->resources->SearchResource<ResourceMesh>(meshesUIDs[i]);
			std::shared_ptr<ResourceMaterial> material = App->resources->SearchResource<ResourceMaterial>(materialsUIDs[i]);
			nodes[i]->meshRenderers.push_back(std::make_pair(mesh, material));
		}
#endif
	}

	resource->SetNodes(nodes);
}

void ModelImporter::ImportNode(const aiScene* scene, const char* filePath, std::shared_ptr<ResourceModel>& resource,
	const aiNode* node, const int parentIdx)
{
	std::string name = node->mName.C_Str();
	const float4x4 transform = (*(float4x4*)&node->mTransformation);
	ResourceModel::Node* resourceNode = new ResourceModel::Node;

	resourceNode->name = &name[0];
	resourceNode->parent = parentIdx;
	resourceNode->transform = transform;

	ENGINE_LOG("Node name: %s", name.c_str());
	if (node->mParent)
	{
		ENGINE_LOG("Parent node name: %s", node->mParent->mName.C_Str());
	}
	ENGINE_LOG("Node parentIdx: %i", parentIdx);

	// loading meshes and materials
	for (int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		ENGINE_LOG("Importing mesh %s", mesh->mName.C_Str());
		ENGINE_LOG("Importing material %s", material->GetName().C_Str());

		std::shared_ptr<ResourceMesh> resourceMesh = ImportMesh(mesh, filePath, i);
		std::shared_ptr<ResourceMaterial> resourceMaterial = ImportMaterial(material, filePath, i);

		resourceNode->meshRenderers.push_back(std::make_pair(resourceMesh, resourceMaterial));
	}
	resource->AppendNode(resourceNode);

	ENGINE_LOG("\n", parentIdx);

	int newParentIdx = resource->GetNumNodes() - 1;

	for (int i = 0; i < node->mNumChildren; ++i)
	{
		ImportNode(scene, filePath, resource, node->mChildren[i], newParentIdx);
	}
}

std::shared_ptr<ResourceMesh> ModelImporter::ImportMesh(const aiMesh* mesh, const char* filePath, int iteration)
{
	char* fileBuffer{};
	unsigned int size = 0;

	SaveInfoMesh(mesh, fileBuffer, size);

	std::string name = mesh->mName.C_Str();
	std::string meshPath = MESHES_PATH + name + "_" + std::to_string(iteration) + MESH_EXTENSION;

	App->fileSystem->Save(meshPath.c_str(),fileBuffer,size);
	std::shared_ptr<ResourceMesh> resourceMesh = std::dynamic_pointer_cast<ResourceMesh>(App->resources->ImportResource(meshPath));

	return resourceMesh;
}

std::shared_ptr<ResourceMaterial> ModelImporter::ImportMaterial(const aiMaterial* material, const char* filePath,
	int iteration)
{
	aiString file;

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

	if (material->GetTexture(aiTextureType_NORMALS, 0, &file) == AI_SUCCESS)
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
	std::string name = material->GetName().C_Str();
	std::string materialPath = MATERIAL_PATH + name + "_" + std::to_string(iteration)
		+ MATERIAL_EXTENSION;

	App->fileSystem->Save(materialPath.c_str(), fileBuffer, size);
	std::shared_ptr<ResourceMaterial> resourceMaterial =
		std::dynamic_pointer_cast<ResourceMaterial>(App->resources->ImportResource(materialPath));

	delete fileBuffer;

	return resourceMaterial;
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
			{
				dataBuffer = TEXTURES_PATH + name;
			}
		}
		else
		{
			dataBuffer = path + name;
		}
	}
	else
	{
		dataBuffer = std::string(file.data);
	}

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

