#include "StdAfx.h"

#include "ModelImporter.h"

#include "Application.h"
#include "DataModels/Resources/ResourceAnimation.h"
#include "DataModels/Resources/ResourceMaterial.h"
#include "DataModels/Resources/ResourceMesh.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"
#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"

#include "Math/float3.h"

#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

void myCallback(const char* msg, char*)
{
	LOG_VERBOSE("[assimp] {}", msg);
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

	LOG_VERBOSE("Import Model from {}", filePath);

	const aiScene* scene =
		aiImportFile(filePath, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);
	if (scene)
	{
		ImportNode(scene, filePath, resource, scene->mRootNode, -1, float4x4::identity);
		ImportAnimations(scene, resource);
		aiReleaseImport(scene);

		char* buffer{};
		unsigned int size;
		Save(resource, buffer, size);
		App->GetModule<ModuleFileSystem>()->Save(
			(resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), buffer, size);

		delete buffer;
	}
	else
	{
		LOG_ERROR("Error loading {}: {}", filePath, aiGetErrorString());
	}
}

void ModelImporter::Save(const std::shared_ptr<ResourceModel>& resource, char*& fileBuffer, unsigned int& size)
{
#ifdef ENGINE
	// Update Meta
	std::string metaPath = resource->GetAssetsPath() + META_EXTENSION;
	char* metaBuffer = {};
	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();
	fileSystem->Load(metaPath.c_str(), metaBuffer);
	rapidjson::Document doc;
	Json meta(doc, doc);
	meta.fromBuffer(metaBuffer);
	delete metaBuffer;

	Json jsonMeshes = meta["MeshesAssetPaths"];
	Json jsonMat = meta["MatAssetPaths"];
	Json jsonAnims = meta["AnimAssetPaths"];
	unsigned int countMeshes = 0;
	unsigned int countMat = 0;
	unsigned int countAnim = 0;
#endif

	size = (sizeof(float4x4) + sizeof(int) + sizeof(unsigned int) * 2) * resource->GetNumNodes();

	for (ResourceModel::Node* node : resource->GetNodes())
	{
		size += sizeof(UID) * 2 * node->meshRenderers.size();
		size += sizeof(char) * node->name.size();
	}

	size += sizeof(UID) * resource->GetNumAnimations();

	unsigned int header[2] = { (unsigned int) resource->GetNumNodes(), (unsigned int) resource->GetNumAnimations() };

	size += sizeof(header);

	char* cursor = new char[size]{};

	fileBuffer = cursor;

	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

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

	std::vector<UID> animationsUIDs;
	animationsUIDs.reserve(resource->GetNumAnimations());
	for (int i = 0; i < resource->GetNumAnimations(); i++)
	{
#ifdef ENGINE
		jsonAnims[countAnim] = resource->GetAnimations()[i]->GetAssetsPath().c_str();
		++countAnim;
#endif
		animationsUIDs.push_back(resource->GetAnimations()[i]->GetUID());
		bytes = sizeof(UID);
		memcpy(cursor, &(animationsUIDs[0]), bytes);
		cursor += bytes;
	}

#ifdef ENGINE
	rapidjson::StringBuffer buffer;
	meta.toBuffer(buffer);
	fileSystem->Save(metaPath.c_str(), buffer.GetString(), (unsigned int) buffer.GetSize());
#endif
}

void ModelImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceModel> resource)
{
#ifdef ENGINE
	// Update Meta
	std::string metaPath = resource->GetAssetsPath() + META_EXTENSION;
	char* metaBuffer = {};
	App->GetModule<ModuleFileSystem>()->Load(metaPath.c_str(), metaBuffer);
	rapidjson::Document doc;
	Json meta(doc, doc);
	meta.fromBuffer(metaBuffer);
	delete metaBuffer;

	Json jsonMeshes = meta["MeshesAssetPaths"];
	Json jsonMat = meta["MatAssetPaths"];
	Json jsonAnims = meta["AnimAssetPaths"];
	unsigned int countMeshes = 0;
	unsigned int countMat = 0;
	unsigned int countAnim = 0;
#endif

	unsigned int header[2];
	unsigned int bytes = sizeof(header);
	memcpy(header, fileBuffer, bytes);
	fileBuffer += bytes;

	std::vector<ResourceModel::Node*> nodes;
	nodes.reserve(header[0]);

	for (unsigned int i = 0; i < header[0]; ++i)
	{
		ResourceModel::Node* node = new ResourceModel::Node;

		unsigned int nodeHeader[2];
		bytes = sizeof(nodeHeader);
		memcpy(nodeHeader, fileBuffer, bytes);
		fileBuffer += bytes;

		char* name = new char[nodeHeader[0]]{};
		bytes = sizeof(char) * (unsigned int) nodeHeader[0];
		memcpy(name, fileBuffer, bytes);
		fileBuffer += bytes;
		node->name = std::string(name, nodeHeader[0]);
		delete[] name;

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

#ifdef ENGINE
		ModuleResources* resources = App->GetModule<ModuleResources>();
		for (int i = 0; i < nodeHeader[1]; i++)
		{
			std::string meshPath = jsonMeshes[countMeshes];
			std::string matPath = jsonMat[countMat];

			std::shared_ptr<ResourceMesh> mesh =
				App->GetModule<ModuleResources>()->RequestResource<ResourceMesh>(meshPath);
			std::shared_ptr<ResourceMaterial> material =
				App->GetModule<ModuleResources>()->RequestResource<ResourceMaterial>(matPath);

			node->meshRenderers.push_back(std::make_pair(mesh, material));

			++countMeshes;
			++countMat;

			fileBuffer += sizeof(UID) * 2;
		}
		nodes.push_back(node);
#else
		UID* meshesPointer = new UID[nodeHeader[1]];
		bytes = sizeof(UID) * (unsigned int) nodeHeader[1];
		memcpy(meshesPointer, fileBuffer, bytes);
		std::vector<UID> meshesUIDs(meshesPointer, meshesPointer + nodeHeader[1]);
		delete[] meshesPointer;
		fileBuffer += bytes;

		UID* materialsPointer = new UID[nodeHeader[1]];
		bytes = sizeof(UID) * (unsigned int) nodeHeader[1];
		memcpy(materialsPointer, fileBuffer, bytes);
		std::vector<UID> materialsUIDs(materialsPointer, materialsPointer + nodeHeader[1]);
		delete[] materialsPointer;
		fileBuffer += bytes;

		for (int i = 0; i < meshesUIDs.size(); i++)
		{
			std::shared_ptr<ResourceMesh> mesh =
				App->GetModule<ModuleResources>()->SearchResource<ResourceMesh>(meshesUIDs[i]);
			std::shared_ptr<ResourceMaterial> material =
				App->GetModule<ModuleResources>()->SearchResource<ResourceMaterial>(materialsUIDs[i]);
			nodes[i]->meshRenderers.push_back(std::make_pair(mesh, material));
		}
#endif
	}
	resource->SetNodes(nodes);

	std::vector<std::shared_ptr<ResourceAnimation>> animations;
	animations.reserve(header[1]);

#ifdef ENGINE
	for (unsigned int i = 0; i < header[1]; ++i)
	{
		std::string animPath = jsonAnims[countAnim];

		std::shared_ptr<ResourceAnimation> anim =
			App->GetModule<ModuleResources>()->RequestResource<ResourceAnimation>(animPath);
		animations.push_back(anim);
		++countAnim;
		fileBuffer += sizeof(UID);
	}
#else
	UID* animationsPointer = new UID[header[1]];
	bytes = sizeof(UID) * (unsigned int) header[1];
	memcpy(animationsPointer, fileBuffer, bytes);
	std::vector<UID> animationsUIDs(animationsPointer, animationsPointer + header[1]);
	delete[] animationsPointer;
	fileBuffer += bytes;
	for (int i = 0; i < animationsUIDs.size(); i++)
	{
		std::shared_ptr<ResourceAnimation> anim =
			App->GetModule<ModuleResources>()->SearchResource<ResourceAnimation>(animationsUIDs[i]);
		animations.push_back(anim);
	}
#endif
	resource->SetAnimations(animations);
}

void ModelImporter::ImportAnimations(const aiScene* scene, const std::shared_ptr<ResourceModel>& resource)
{
	std::vector<std::shared_ptr<ResourceAnimation>> animations;
	animations.reserve(scene->mNumAnimations);

	for (int i = 0; i < scene->mNumAnimations; ++i)
	{
		aiAnimation* animation = scene->mAnimations[i];

		char* fileBuffer{};
		unsigned int size = 0;
		ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();
		SaveInfoAnimation(animation, fileBuffer, size);

		std::string animationName = animation->mName.C_Str();

		int pos = 0;
		std::replace(std::begin(animationName), std::end(animationName), '|', '_');
		std::string animationPath =
			ANIMATION_PATH + resource->GetFileName() + "." + animationName + ANIMATION_EXTENSION;

		fileSystem->Save(animationPath.c_str(), fileBuffer, size);
		std::shared_ptr<ResourceAnimation> resourceAnimation = std::dynamic_pointer_cast<ResourceAnimation>(
			App->GetModule<ModuleResources>()->ImportResource(animationPath));
		animations.push_back(resourceAnimation);
	}
	resource->SetAnimations(animations);
}

void ModelImporter::ImportNode(const aiScene* scene,
							   const char* filePath,
							   const std::shared_ptr<ResourceModel>& resource,
							   const aiNode* node,
							   int parentIdx,
							   const float4x4& accTransform)
{
	std::string name = node->mName.C_Str();
	const float4x4 transform = (*(float4x4*) &node->mTransformation);

	if (name.find("$AssimpFbx$") != std::string::npos)
	{
		for (unsigned int i = 0; i < node->mNumChildren; ++i)
		{
			const float4x4& newAcctransform = accTransform * transform;

			ImportNode(scene, filePath, resource, node->mChildren[i], parentIdx, newAcctransform);
		}
	}
	else
	{
		ResourceModel::Node* resourceNode = new ResourceModel::Node;

		resourceNode->name = &name[0];
		resourceNode->parent = parentIdx;
		resourceNode->transform = transform * accTransform;

		LOG_VERBOSE("Node name: {}", name);
		if (node->mParent)
		{
			LOG_VERBOSE("Parent node name: {}", node->mParent->mName.C_Str());
		}
		LOG_VERBOSE("Node parentIdx: {}", parentIdx);

		float3 pos;
		float4x4 rot;
		float3 scale;

		transform.Decompose(pos, rot, scale);

		LOG_VERBOSE("Transform:\n\tpos: ({}, {}, {})\trot: ({}, {}, {})\t scale: ({}, {}, {})",
					pos.x,
					pos.y,
					pos.z,
					RadToDeg(rot.ToEulerXYZ().x),
					RadToDeg(rot.ToEulerXYZ().y),
					RadToDeg(rot.ToEulerXYZ().z),
					scale.x,
					scale.y,
					scale.z);

		// loading meshes and materials
		for (int i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			LOG_VERBOSE("Importing mesh {}", mesh->mName.C_Str());
			LOG_VERBOSE("Importing material {}", material->GetName().C_Str());

			std::shared_ptr<ResourceMesh> resourceMesh = ImportMesh(mesh, filePath, i);
			std::shared_ptr<ResourceMaterial> resourceMaterial = ImportMaterial(material, filePath, i);

			resourceNode->meshRenderers.push_back(std::make_pair(resourceMesh, resourceMaterial));
		}
		resource->AppendNode(resourceNode);

		LOG_VERBOSE("\n{}", parentIdx);

		int newParentIdx = resource->GetNumNodes() - 1;

		for (int i = 0; i < node->mNumChildren; ++i)
		{
			ImportNode(scene, filePath, resource, node->mChildren[i], newParentIdx, float4x4::identity);
		}
	}
}

std::shared_ptr<ResourceMesh> ModelImporter::ImportMesh(const aiMesh* mesh, const char* filePath, int iteration)
{
	char* fileBuffer{};
	unsigned int size = 0;

	SaveInfoMesh(mesh, fileBuffer, size);

	std::string name = mesh->mName.C_Str();
	std::string meshPath = MESHES_PATH + App->GetModule<ModuleFileSystem>()->GetFileName(filePath) + "." + name + "_" +
						   std::to_string(iteration) + MESH_EXTENSION;

	App->GetModule<ModuleFileSystem>()->Save(meshPath.c_str(), fileBuffer, size);
	std::shared_ptr<ResourceMesh> resourceMesh =
		std::dynamic_pointer_cast<ResourceMesh>(App->GetModule<ModuleResources>()->ImportResource(meshPath));

	return resourceMesh;
}

std::shared_ptr<ResourceMaterial>
	ModelImporter::ImportMaterial(const aiMaterial* material, const char* filePath, int iteration)
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

	if (material->GetTexture(aiTextureType_NORMALS, 0, &file) == AI_SUCCESS)
	{
		std::string normalPath = "";

		CheckPathMaterial(filePath, file, normalPath);

		if (normalPath != "")
		{
			pathTextures[1] = normalPath;
		}
	}

	if (material->GetTexture(aiTextureType_LIGHTMAP, 0, &file) == AI_SUCCESS)
	{
		std::string occlusionPath = "";

		CheckPathMaterial(filePath, file, occlusionPath);

		if (occlusionPath != "")
		{
			pathTextures[2] = occlusionPath;
		}
	}

	if (material->GetTexture(aiTextureType_SPECULAR, 0, &file) == AI_SUCCESS)
	{
		std::string specularPath = "";

		CheckPathMaterial(filePath, file, specularPath);

		if (specularPath != "")
		{
			pathTextures[3] = specularPath;
		}
	}

	char* fileBuffer{};
	unsigned int size = 0;

	App->GetModule<ModuleFileSystem>()->SaveInfoMaterial(pathTextures, fileBuffer, size);
	std::string name = material->GetName().C_Str();
	std::string materialPath = MATERIAL_PATH + App->GetModule<ModuleFileSystem>()->GetFileName(filePath) + name + "_" +
							   std::to_string(iteration) + MATERIAL_EXTENSION;

	App->GetModule<ModuleFileSystem>()->Save(materialPath.c_str(), fileBuffer, size);
	std::shared_ptr<ResourceMaterial> resourceMaterial =
		std::dynamic_pointer_cast<ResourceMaterial>(App->GetModule<ModuleResources>()->ImportResource(materialPath));

	delete fileBuffer;

	return resourceMaterial;
}

void ModelImporter::CheckPathMaterial(const char* filePath, const aiString& file, std::string& dataBuffer)
{
	struct stat buffer
	{
	};
	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();
	std::string name = fileSystem->GetFileName(file.data);
	name += fileSystem->GetFileExtension(file.data);

	// Cheking by name
	if (stat(file.data, &buffer) != 0)
	{
		std::string path = fileSystem->GetPathWithoutFile(filePath);
		// Checking in the original fbx folder
		if (stat((path + name).c_str(), &buffer) != 0)
		{
			// Cheking in asset textures folder
			if (stat((TEXTURES_PATH + name).c_str(), &buffer) != 0)
			{
				LOG_WARNING("Texture not found!");
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

void ModelImporter::SaveInfoAnimation(const aiAnimation* animation, char*& fileBuffer, unsigned int& size)
{
	unsigned int header[1] = { animation->mNumChannels };

	size = (sizeof(unsigned int) * 3) * animation->mNumChannels + sizeof(header) + sizeof(double);

	for (int i = 0; i < animation->mNumChannels; ++i)
	{
		size += sizeof(float3) * animation->mChannels[i]->mNumPositionKeys;
		size += sizeof(Quat) * animation->mChannels[i]->mNumRotationKeys;
		size += sizeof(char) * animation->mChannels[i]->mNodeName.length;
	}

	char* cursor = new char[size]{};

	fileBuffer = cursor;

	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

	bytes = sizeof(double);
	memcpy(cursor, &animation->mDuration, bytes);

	cursor += bytes;

	for (int i = 0; i < animation->mNumChannels; ++i)
	{
		aiNodeAnim* nodeAnim = animation->mChannels[i];

		unsigned int nodeHeader[3] = {
			nodeAnim->mNodeName.length,
			nodeAnim->mNumPositionKeys,
			nodeAnim->mNumRotationKeys,
		};

		bytes = sizeof(nodeHeader);
		memcpy(cursor, nodeHeader, bytes);

		cursor += bytes;

		bytes = sizeof(char) * nodeAnim->mNodeName.length;
		memcpy(cursor, &(nodeAnim->mNodeName.data[0]), bytes);

		cursor += bytes;

		if (nodeAnim->mPositionKeys != nullptr)
		{
			for (int i = 0; i < nodeAnim->mNumPositionKeys; ++i)
			{
				bytes = sizeof(float3);
				memcpy(cursor, &(nodeAnim->mPositionKeys[i].mValue), bytes);

				cursor += bytes;
			}
		}

		if (nodeAnim->mRotationKeys != nullptr)
		{
			for (int i = 0; i < nodeAnim->mNumRotationKeys; ++i)
			{
				bytes = sizeof(Quat);
				Quat* rotation = new Quat(nodeAnim->mRotationKeys[i].mValue.x,
										  nodeAnim->mRotationKeys[i].mValue.y,
										  nodeAnim->mRotationKeys[i].mValue.z,
										  nodeAnim->mRotationKeys[i].mValue.w);

				memcpy(cursor, rotation, bytes);
				delete rotation;

				cursor += bytes;
			}
		}
	}
}

void ModelImporter::SaveInfoMesh(const aiMesh* ourMesh, char*& fileBuffer, unsigned int& size)
{
	unsigned int numIndexes = 3;

	unsigned int hasTangents = ourMesh->mTangents != nullptr;

	unsigned int header[5] = {
		ourMesh->mNumFaces, ourMesh->mNumVertices, ourMesh->mNumBones, ourMesh->mMaterialIndex, hasTangents
	};

	unsigned int sizeOfVectors = sizeof(float3) * ourMesh->mNumVertices;
	unsigned int numOfVectors = 3;
	if (hasTangents)
	{
		numOfVectors = 4;
	}
	size = sizeof(header) + ourMesh->mNumFaces * (sizeof(unsigned int) * numIndexes) +
		   static_cast<unsigned long long>(sizeOfVectors) * static_cast<unsigned long long>(numOfVectors) +
		   ourMesh->mNumBones * (sizeof(aiMatrix4x4) + sizeof(unsigned int) * 2);

	for (unsigned int i = 0; i < ourMesh->mNumBones; ++i)
	{
		size += ourMesh->mBones[i]->mName.length;
		size += ourMesh->mBones[i]->mNumWeights * (sizeof(unsigned int) + sizeof(float));
	}

	char* cursor = new char[size]{};

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

	for (unsigned int i = 0; i < ourMesh->mNumBones; ++i)
	{
		bytes = sizeof(aiMatrix4x4);
		memcpy(cursor, &(ourMesh->mBones[i]->mOffsetMatrix), bytes);

		cursor += bytes;

		bytes = sizeof(unsigned int);
		unsigned int sizeNameHeader = ourMesh->mBones[i]->mName.length;
		memcpy(cursor, &sizeNameHeader, bytes);

		cursor += bytes;

		bytes = sizeNameHeader;
		memcpy(cursor, ourMesh->mBones[i]->mName.C_Str(), bytes);

		cursor += bytes;

		bytes = sizeof(unsigned int);
		memcpy(cursor, &(ourMesh->mBones[i]->mNumWeights), bytes);

		cursor += bytes;

		for (unsigned int j = 0; j < ourMesh->mBones[i]->mNumWeights; ++j)
		{
			bytes = sizeof(unsigned int);
			memcpy(cursor, &(ourMesh->mBones[i]->mWeights[j].mVertexId), bytes);

			cursor += bytes;

			bytes = sizeof(float);
			memcpy(cursor, &(ourMesh->mBones[i]->mWeights[j].mWeight), bytes);

			cursor += bytes;
		}
	}
}
