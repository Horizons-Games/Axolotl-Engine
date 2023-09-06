#include "StdAfx.h"

#include "ModuleResources.h"

#include "Application.h"
#include "ModuleFileSystem.h"

#include "FileSystem/Importers/AnimationImporter.h"
#include "FileSystem/Importers/CubemapImporter.h"
#include "FileSystem/Importers/MaterialImporter.h"
#include "FileSystem/Importers/MeshImporter.h"
#include "FileSystem/Importers/ModelImporter.h"
#include "FileSystem/Importers/NavMeshImporter.h"
#include "FileSystem/Importers/ParticleSystemImporter.h"
#include "FileSystem/Importers/SkyBoxImporter.h"
#include "FileSystem/Importers/StateMachineImporter.h"
#include "FileSystem/Importers/TextureImporter.h"
#include "FileSystem/UIDGenerator.h"

#include "ParticleSystem/ParticleEmitter.h"
#include "Resources/EditorResource/EditorResource.h"
#include "Resources/ResourceAnimation.h"
#include "Resources/ResourceCubemap.h"
#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceParticleSystem.h"
#include "Resources/ResourceSkyBox.h"
#include "Resources/ResourceTexture.h"

const std::string ModuleResources::assetsFolder = "Assets/";
const std::string ModuleResources::libraryFolder = "Lib/";

// creator and destructor can't be inlined
// because we are using unique pointers with forward declarations
ModuleResources::ModuleResources() : monitorResources(false)
{
}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Init()
{
	monitorResources = true;
	modelImporter = std::make_unique<ModelImporter>();
	textureImporter = std::make_unique<TextureImporter>();
	meshImporter = std::make_unique<MeshImporter>();
	navMeshImporter = std::make_unique<NavMeshImporter>();
	materialImporter = std::make_unique<MaterialImporter>();
	skyboxImporter = std::make_unique<SkyBoxImporter>();
	cubemapImporter = std::make_unique<CubemapImporter>();
	animationImporter = std::make_unique<AnimationImporter>();
	stateMachineImporter = std::make_unique<StateMachineImporter>();
	particleSystemImporter = std::make_unique<ParticleSystemImporter>();
	CreateAssetAndLibFolders();

#ifdef ENGINE
	CreateAssetAndLibFolders();
	monitorThread = std::thread(&ModuleResources::MonitorResources, this);
#endif
	return true;
}

bool ModuleResources::CleanUp()
{
#ifdef ENGINE
	monitorResources = false;
	monitorThread.join();
#else
	resourcesBin.clear();
#endif
	resources.clear();
	return true;
}

void ModuleResources::CreateDefaultResource(ResourceType type, const std::string& fileName)
{
	std::shared_ptr<Resource> importedRes;
	std::string assetsPath = CreateAssetsPath(fileName, type);
	switch (type)
	{
		case ResourceType::NavMesh:
			assetsPath += NAVMESH_EXTENSION;
			importedRes = CreateNewResource("DefaultNavMesh", assetsPath, ResourceType::NavMesh);
			CreateMetaFileOfResource(importedRes);
			navMeshImporter->Import(assetsPath.c_str(), std::dynamic_pointer_cast<ResourceNavMesh>(importedRes));
			// TODO when we finish the ResourceNavMesh we need to create a PreMade Default or other form to create the
			// resource
			break;
		case ResourceType::Material:
			assetsPath += MATERIAL_EXTENSION;
			App->GetModule<ModuleFileSystem>()->CopyFileInAssets("Source/PreMades/Default.mat", assetsPath);
			ImportResource(assetsPath);
			break;
		case ResourceType::StateMachine:
			assetsPath += STATEMACHINE_EXTENSION;
			/*importedRes = CreateNewResource("DefaultStateMachine", assetsPath, ResourceType::StateMachine);
			stateMachineImporter->Import(assetsPath.c_str(),
			std::dynamic_pointer_cast<ResourceStateMachine>(importedRes));*/
			App->GetModule<ModuleFileSystem>()->CopyFileInAssets("Source/PreMades/StateMachineDefault.state",
																 assetsPath);
			ImportResource(assetsPath);
			break;
		case ResourceType::ParticleSystem:
			assetsPath += PARTICLESYSTEM_EXTENSION;
			/*importedRes = CreateNewResource("DefaultParticleSystem", assetsPath, ResourceType::ParticleSystem);
			particleSystemImporter->Import(assetsPath.c_str(),
			std::dynamic_pointer_cast<ResourceParticleSystem>(importedRes));*/
			App->GetModule<ModuleFileSystem>()->CopyFileInAssets("Source/PreMades/ParticleSystemDefault.particle",
																 assetsPath);
			ImportResource(assetsPath);
			break;
		default:
			break;
	}
}

// Creates Binary and Meta from an Asset original path
std::shared_ptr<Resource> ModuleResources::ImportResource(const std::string& originalPath)
{
	ResourceType type = FindTypeByExtension(originalPath);
	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();
	if (type == ResourceType::Unknown)
	{
		LOG_WARNING("Extension not supported");
	}
	std::string fileName = fileSystem->GetFileName(originalPath);
	std::string extension = fileSystem->GetFileExtension(originalPath);
	std::string assetsPath = CreateAssetsPath(fileName + extension, type);

	bool resourceExists = fileSystem->Exists(assetsPath.c_str());
	if (!resourceExists)
		fileSystem->CopyFileInAssets(originalPath, assetsPath);

	std::shared_ptr<Resource> importedRes = CreateNewResource(fileName, assetsPath, type);
	CreateMetaFileOfResource(importedRes);
	ImportResourceFromSystem(assetsPath, importedRes, importedRes->GetType());

	return importedRes;
}

std::future<std::shared_ptr<Resource>> ModuleResources::ImportThread(const std::string& originalPath)
{
	return std::async(std::launch::async,
					  [=]()
					  {
						  return RequestResource(originalPath);
					  });
}

std::shared_ptr<Resource>
	ModuleResources::CreateNewResource(const std::string& fileName, const std::string& assetsPath, ResourceType type)
{
	UID uid = UniqueID::GenerateUID();
	const std::string libraryPath = CreateLibraryPath(uid, type);
	return CreateResourceOfType(uid, fileName, assetsPath, libraryPath, type);
}

std::shared_ptr<Resource> ModuleResources::CreateResourceOfType(UID uid,
																const std::string& fileName,
																const std::string& assetsPath,
																const std::string& libraryPath,
																ResourceType type)
{
	// at this point, it might be easier to make this erase_if call each post update,
	// but I'd need to profile it in order to not get comments about performance, and I don't wanna
	auto customDeleter = [this](Resource* pointer)
	{
		std::erase_if(resources,
					  [](const auto& uidAndRes)
					  {
						  return uidAndRes.second.expired();
					  });
		delete pointer;
	};

#ifdef ENGINE
	switch (type)
	{
		case ResourceType::Model:
			return std::shared_ptr<EditorResource<ResourceModel>>(
				new EditorResource<ResourceModel>(uid, fileName, assetsPath, libraryPath), customDeleter);
		case ResourceType::Texture:
			return std::shared_ptr<EditorResource<ResourceTexture>>(
				new EditorResource<ResourceTexture>(uid, fileName, assetsPath, libraryPath), customDeleter);
		case ResourceType::Mesh:
			return std::shared_ptr<EditorResource<ResourceMesh>>(
				new EditorResource<ResourceMesh>(uid, fileName, assetsPath, libraryPath), customDeleter);
		case ResourceType::NavMesh:
			return std::shared_ptr<EditorResource<ResourceNavMesh>>(
				new EditorResource<ResourceNavMesh>(uid, fileName, assetsPath, libraryPath), customDeleter);
			break;
		case ResourceType::Scene:
			// good luck with that :)
			AXO_TODO("Implement resource scene")
			return nullptr;
		case ResourceType::Material:
			return std::shared_ptr<EditorResource<ResourceMaterial>>(
				new EditorResource<ResourceMaterial>(uid, fileName, assetsPath, libraryPath), customDeleter);
		case ResourceType::SkyBox:
			return std::shared_ptr<EditorResource<ResourceSkyBox>>(
				new EditorResource<ResourceSkyBox>(uid, fileName, assetsPath, libraryPath), customDeleter);
		case ResourceType::Cubemap:
			return std::shared_ptr<EditorResource<ResourceCubemap>>(
				new EditorResource<ResourceCubemap>(uid, fileName, assetsPath, libraryPath), customDeleter);
		case ResourceType::Animation:
			return std::shared_ptr<EditorResource<ResourceAnimation>>(
				new EditorResource<ResourceAnimation>(uid, fileName, assetsPath, libraryPath), customDeleter);
		case ResourceType::StateMachine:
			return std::shared_ptr<EditorResource<ResourceStateMachine>>(
				new EditorResource<ResourceStateMachine>(uid, fileName, assetsPath, libraryPath), customDeleter);
		case ResourceType::ParticleSystem:
			return std::shared_ptr<EditorResource<ResourceParticleSystem>>(
				new EditorResource<ResourceParticleSystem>(uid, fileName, assetsPath, libraryPath), customDeleter);
		default:
			return nullptr;
	}
#else
	switch (type)
	{
		case ResourceType::Model:
			return std::shared_ptr<ResourceModel>(new ResourceModel(uid, fileName, assetsPath, libraryPath),
												  customDeleter);
			break;
		case ResourceType::Texture:
			return std::shared_ptr<ResourceTexture>(new ResourceTexture(uid, fileName, assetsPath, libraryPath),
													customDeleter);
			break;
		case ResourceType::Mesh:
			return std::shared_ptr<ResourceMesh>(new ResourceMesh(uid, fileName, assetsPath, libraryPath),
												 customDeleter);
			break;
		case ResourceType::NavMesh:
			return std::shared_ptr<ResourceNavMesh>(new ResourceNavMesh(uid, fileName, assetsPath, libraryPath),
													customDeleter);
			break;
		case ResourceType::Scene:
			AXO_TODO("Implement resource scene")
			return nullptr;
		case ResourceType::Material:
			return std::shared_ptr<ResourceMaterial>(new ResourceMaterial(uid, fileName, assetsPath, libraryPath),
													 customDeleter);
			break;
		case ResourceType::SkyBox:
			return std::shared_ptr<ResourceSkyBox>(new ResourceSkyBox(uid, fileName, assetsPath, libraryPath),
												   customDeleter);
			break;
		case ResourceType::Cubemap:
			return std::shared_ptr<ResourceCubemap>(new ResourceCubemap(uid, fileName, assetsPath, libraryPath),
													customDeleter);
			break;
		case ResourceType::Animation:
			return std::shared_ptr<ResourceAnimation>(new ResourceAnimation(uid, fileName, assetsPath, libraryPath),
													  customDeleter);
			break;
		case ResourceType::StateMachine:
			return std::shared_ptr<ResourceStateMachine>(
				new ResourceStateMachine(uid, fileName, assetsPath, libraryPath), customDeleter);
			break;
		case ResourceType::ParticleSystem:
			return std::shared_ptr<ResourceParticleSystem>(
				new ResourceParticleSystem(uid, fileName, assetsPath, libraryPath), customDeleter);
			break;
		default:
			return nullptr;
	}
#endif // ENGINE
}

void ModuleResources::AddResource(std::shared_ptr<Resource>& resource, const std::string& originalPath)
{
	CreateMetaFileOfResource(resource);

	ImportResourceFromSystem(originalPath, resource, resource->GetType());

	UID uid = resource->GetUID();
	resources.insert({ uid, resource });
}

void ModuleResources::DeleteResource(const std::shared_ptr<EditorResourceInterface>& resToDelete)
{
	// resToDelete->MarkToDelete();
	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();

	std::string libPath = resToDelete->GetLibraryPath() + GENERAL_BINARY_EXTENSION;
	std::string metaPath = resToDelete->GetLibraryPath() + META_EXTENSION;
	fileSystem->Delete(metaPath.c_str());
	fileSystem->Delete(libPath.c_str());

	if (resToDelete)
	{
		if (resToDelete->GetType() == ResourceType::Model)
		{
			std::shared_ptr<ResourceModel> modelToDelete = std::dynamic_pointer_cast<ResourceModel>(resToDelete);
			for (const std::shared_ptr<ResourceMesh>& mesh : modelToDelete->GetMeshes())
			{
				DeleteResource(std::dynamic_pointer_cast<EditorResourceInterface>(mesh));
			}
		}
		else if (resToDelete->GetType() == ResourceType::Mesh) // mesh should not have an asset
		{
			fileSystem->Delete(resToDelete->GetAssetsPath().c_str());
		}
	}

	// resources.erase(resToDelete->GetUID());
}

std::shared_ptr<Resource> ModuleResources::LoadResourceStored(const char* filePath, const char* fileNameToStore)
{
	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();
	std::vector<std::string> files = fileSystem->ListFiles(filePath);
	for (size_t i = 0; i < files.size(); i++)
	{
		std::string path(filePath);
		path += '/' + files[i];
		const char* file = path.c_str();
		if (fileSystem->IsDirectory(file))
		{
			std::shared_ptr<Resource> resource = LoadResourceStored(file, fileNameToStore);
			if (resource)
				return resource;
		}
		else
		{
			if (fileSystem->GetFileName(file) == fileNameToStore)
			{
				std::string fileName = fileSystem->GetFileName(file);
				UID uid = std::stoull(fileName.c_str(), NULL, 0);
				ResourceType type = FindTypeByFolder(file);
				std::shared_ptr<Resource> resource =
					CreateResourceOfType(uid, fileName, "", fileSystem->GetPathWithoutExtension(file), type);

				ImportResourceFromLibrary(resource);

				return resource;
			}
		}
	}

	return nullptr;
}

void ModuleResources::ImportResourceFromLibrary(std::shared_ptr<Resource>& resource)
{
	std::string libPath = resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION;
	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();

	if (resource->GetType() != ResourceType::Unknown && fileSystem->Exists(libPath.c_str()))
	{
		if (resource != nullptr)
		{
			char* binaryBuffer = {};
			fileSystem->Load(libPath.c_str(), binaryBuffer);

			switch (resource->GetType())
			{
				case ResourceType::Model:
					modelImporter->Load(binaryBuffer, std::dynamic_pointer_cast<ResourceModel>(resource));
					break;
				case ResourceType::Texture:
					textureImporter->Load(binaryBuffer, std::dynamic_pointer_cast<ResourceTexture>(resource));
					break;
				case ResourceType::Mesh:
					meshImporter->Load(binaryBuffer, std::dynamic_pointer_cast<ResourceMesh>(resource));
					break;
				case ResourceType::NavMesh:
					navMeshImporter->Load(binaryBuffer, std::dynamic_pointer_cast<ResourceNavMesh>(resource));
					break;
				case ResourceType::Scene:
					break;
				case ResourceType::Material:
					materialImporter->Load(binaryBuffer, std::dynamic_pointer_cast<ResourceMaterial>(resource));
					break;
				case ResourceType::SkyBox:
					skyboxImporter->Load(binaryBuffer, std::dynamic_pointer_cast<ResourceSkyBox>(resource));
					break;
				case ResourceType::Cubemap:
					cubemapImporter->Load(binaryBuffer, std::dynamic_pointer_cast<ResourceCubemap>(resource));
					break;
				case ResourceType::Animation:
					animationImporter->Load(binaryBuffer, std::dynamic_pointer_cast<ResourceAnimation>(resource));
					break;
				case ResourceType::StateMachine:
					stateMachineImporter->Load(binaryBuffer, std::dynamic_pointer_cast<ResourceStateMachine>(resource));
					break;
				case ResourceType::ParticleSystem:
					particleSystemImporter->Load(binaryBuffer,
												 std::dynamic_pointer_cast<ResourceParticleSystem>(resource));
					break;
				default:
					break;
			}
			delete binaryBuffer;
			return;
		}
	}
}

void ModuleResources::ReimportResource(UID resourceUID)
{
	std::shared_ptr<Resource> resource = resources[resourceUID].lock();
	CreateMetaFileOfResource(resource);
	resource->SetChanged(false);
	if (resource->GetType() == ResourceType::Material)
	{
		std::shared_ptr<ResourceMaterial> materialResource = std::dynamic_pointer_cast<ResourceMaterial>(resource);
		rapidjson::Document doc;
		Json mat(doc, doc);
		materialResource->SavePaths(mat);
		rapidjson::StringBuffer buffer;
		mat.toBuffer(buffer);
		App->GetModule<ModuleFileSystem>()->Save(
			materialResource->GetAssetsPath().c_str(), buffer.GetString(), (unsigned int) buffer.GetSize());
	}
	if (resource->GetType() == ResourceType::StateMachine)
	{
		std::shared_ptr<ResourceStateMachine> stateMachineResource =
			std::dynamic_pointer_cast<ResourceStateMachine>(resource);
		char* saveBuffer = {};
		unsigned int size = 0;
		stateMachineImporter->Save(stateMachineResource, saveBuffer, size);
		App->GetModule<ModuleFileSystem>()->Save(stateMachineResource->GetAssetsPath().c_str(), saveBuffer, size);
		delete saveBuffer;
	}
	if (resource->GetType() == ResourceType::ParticleSystem)
	{
		std::shared_ptr<ResourceParticleSystem> particleResource =
			std::dynamic_pointer_cast<ResourceParticleSystem>(resource);
		char* saveBuffer = {};
		unsigned int size = 0;
		particleSystemImporter->Save(particleResource, saveBuffer, size);

		App->GetModule<ModuleFileSystem>()->Save(particleResource->GetAssetsPath().c_str(), saveBuffer, size);
		delete saveBuffer;
	}
	ImportResourceFromSystem(resource->GetAssetsPath(), resource, resource->GetType());
}

void ModuleResources::FillResourceBin(std::shared_ptr<Resource> sharedResource)
{
#ifndef ENGINE
	resourcesBin.push_back(sharedResource);
#endif // !ENGINE
}

void ModuleResources::CreateMetaFileOfResource(std::shared_ptr<Resource>& resource)
{
	std::string metaPath = resource->GetAssetsPath() + META_EXTENSION;
	rapidjson::Document doc;
	Json meta(doc, doc);
	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();

	if (!resource->IsChanged() && fileSystem->Exists(metaPath.c_str()))
	{
		char* metaBuffer = {};
		fileSystem->Load(metaPath.c_str(), metaBuffer);
		meta.fromBuffer(metaBuffer);
		delete metaBuffer;
		resource = CreateResourceOfType(meta["UID"],
										resource->GetFileName(),
										resource->GetAssetsPath(),
										CreateLibraryPath(meta["UID"], resource->GetType()),
										resource->GetType());
		resource->LoadImporterOptions(meta);
		resource->LoadLoadOptions(meta);
	}
	else
	{
		meta["UID"] = resource->GetUID();
		meta["Type"] = GetNameOfType(resource->GetType()).c_str();
		resource->SaveImporterOptions(meta);
		resource->SaveLoadOptions(meta);
		rapidjson::StringBuffer buffer;
		meta.toBuffer(buffer);
		fileSystem->Save(metaPath.c_str(), buffer.GetString(), (unsigned int) buffer.GetSize());
	}
}

void ModuleResources::ImportResourceFromSystem(const std::string& originalPath,
											   std::shared_ptr<Resource>& resource,
											   ResourceType type)
{
	switch (type)
	{
		case ResourceType::Model:
			modelImporter->Import(originalPath.c_str(), std::dynamic_pointer_cast<ResourceModel>(resource));
			break;
		case ResourceType::Texture:
			textureImporter->Import(originalPath.c_str(), std::dynamic_pointer_cast<ResourceTexture>(resource));
			break;
		case ResourceType::Mesh:
			meshImporter->Import(originalPath.c_str(), std::dynamic_pointer_cast<ResourceMesh>(resource));
			break;
		case ResourceType::NavMesh:
			navMeshImporter->Import(originalPath.c_str(), std::dynamic_pointer_cast<ResourceNavMesh>(resource));
			break;
		case ResourceType::Scene:
			break;
		case ResourceType::Material:
			materialImporter->Import(originalPath.c_str(), std::dynamic_pointer_cast<ResourceMaterial>(resource));
			break;
		case ResourceType::SkyBox:
			skyboxImporter->Import(originalPath.c_str(), std::dynamic_pointer_cast<ResourceSkyBox>(resource));
			break;
		case ResourceType::Cubemap:
			cubemapImporter->Import(originalPath.c_str(), std::dynamic_pointer_cast<ResourceCubemap>(resource));
			break;
		case ResourceType::Animation:
			animationImporter->Import(originalPath.c_str(), std::dynamic_pointer_cast<ResourceAnimation>(resource));
			break;
		case ResourceType::StateMachine:
			stateMachineImporter->Import(originalPath.c_str(),
										 std::dynamic_pointer_cast<ResourceStateMachine>(resource));
			break;
		case ResourceType::ParticleSystem:
			particleSystemImporter->Import(originalPath.c_str(),
										   std::dynamic_pointer_cast<ResourceParticleSystem>(resource));
		default:
			break;
	}
}

void ModuleResources::CreateAssetAndLibFolders()
{
	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();
	bool assetsFolderNotCreated = !fileSystem->Exists(ASSETS_PATH);
	if (assetsFolderNotCreated)
	{
		fileSystem->CreateDirectory(ASSETS_PATH);
	}
	bool libraryFolderNotCreated = !fileSystem->Exists(LIB_PATH);
	if (libraryFolderNotCreated)
	{
		fileSystem->CreateDirectory(LIB_PATH);
	}
	// seems there is no easy way to iterate over enum classes in C++ :/
	//(actually there is a library that looks really clean but might be overkill:
	// https://github.com/Neargye/magic_enum)
	// ensure this vector is updated whenever a new type of resource is added
	std::vector<ResourceType> allResourceTypes = {
		ResourceType::Material,		ResourceType::Mesh,	   ResourceType::Model,			ResourceType::Scene,
		ResourceType::Texture,		ResourceType::SkyBox,  ResourceType::Cubemap,		ResourceType::Animation,
		ResourceType::StateMachine, ResourceType::NavMesh, ResourceType::ParticleSystem
	};

	for (ResourceType type : allResourceTypes)
	{
		std::string folderOfType = GetFolderOfType(type);

		std::string assetsFolderOfType = ASSETS_PATH + folderOfType;
		bool assetsFolderOfTypeNotCreated = !fileSystem->Exists(assetsFolderOfType.c_str());
		if (assetsFolderOfTypeNotCreated)
		{
			fileSystem->CreateDirectory(assetsFolderOfType.c_str());
		}

		std::string libraryFolderOfType = LIB_PATH + folderOfType;
		bool libraryFolderOfTypeNotCreated = !fileSystem->Exists(libraryFolderOfType.c_str());
		if (libraryFolderOfTypeNotCreated)
		{
			fileSystem->CreateDirectory(libraryFolderOfType.c_str());
		}
	}
}

void ModuleResources::MonitorResources()
{
	while (monitorResources)
	{
		ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();
		CreateAssetAndLibFolders();

		std::vector<std::shared_ptr<EditorResourceInterface>> toRemove;
		std::vector<std::shared_ptr<Resource>> toImport;
		std::vector<std::shared_ptr<Resource>> toCreateLib;
		std::vector<std::shared_ptr<Resource>> toCreateMeta;

		// create a copy of the current state of the resources cache to avoid concurrency problems
		std::map<UID, std::weak_ptr<Resource>> copyOfResources = resources;

		for (const auto& [uid, weakResource] : copyOfResources)
		{
			std::shared_ptr<Resource> resource = weakResource.lock();
			if (resource == nullptr)
			{
				continue;
			}

			if (resource->GetType() != ResourceType::Mesh && !fileSystem->Exists(resource->GetAssetsPath().c_str()))
			{
				toRemove.push_back(std::dynamic_pointer_cast<EditorResourceInterface>(resource));
			}
			else
			{
				std::string libraryPathWithExtension = fileSystem->GetPathWithExtension(resource->GetLibraryPath());

				if (libraryPathWithExtension.empty() /*file with that name was not found*/ ||
					!fileSystem->Exists(libraryPathWithExtension.c_str()))
				{
					toCreateLib.push_back(resource);
				}
				if (!fileSystem->Exists((resource->GetAssetsPath() + META_EXTENSION).c_str()))
				{
					toCreateMeta.push_back(resource);
				}
				// these type's assets are binary files changed in runtime
				else if (resource->GetType() != ResourceType::Mesh && resource->GetType() != ResourceType::Material)
				{
					long long assetTimestamp = fileSystem->GetModificationDate(resource->GetAssetsPath().c_str());
					long long libTimestamp = fileSystem->GetModificationDate(
						(resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str());
					if (assetTimestamp > libTimestamp)
					{
						toImport.push_back(resource);
					}
				}
			}
		}
		// Remove resources
		for (std::shared_ptr<EditorResourceInterface> resource : toRemove)
		{
			DeleteResource(resource);
			LOG_INFO("{} deleted", resource);
		}
		// Import resources
		for (std::shared_ptr<Resource>& resource : toImport)
		{
			AddResource(resource, resource->GetAssetsPath());
			LOG_INFO("{} imported", resource);
		}
		for (std::shared_ptr<Resource>& resource : toCreateLib)
		{
			ImportResourceFromSystem(resource->GetAssetsPath(), resource, resource->GetType());
			LOG_INFO("{} imported from system", resource);
		}
		for (std::shared_ptr<Resource>& resource : toCreateMeta)
		{
			CreateMetaFileOfResource(resource);
			LOG_INFO("Meta file of {} created", resource);
		}
		std::this_thread::sleep_for(std::chrono::seconds(4));
	}
}

bool ModuleResources::ExistsResourceWithAssetsPath(const std::string& assetsPath, UID& resourceUID)
{
	std::map<UID, std::weak_ptr<Resource>>::iterator it;
	for (it = resources.begin(); it != resources.end(); it++)
	{
		std::shared_ptr<Resource> resourceAsShared = it->second.lock();

		if (resourceAsShared->GetAssetsPath() == assetsPath)
		{
			resourceUID = resourceAsShared->GetUID();
			return true;
		}
	}
	return false;
}

ResourceType ModuleResources::FindTypeByFolder(const std::string& path)
{
	std::string pathWithOutFile = App->GetModule<ModuleFileSystem>()->GetPathWithoutFile(path);
	std::string::size_type libPathPos = pathWithOutFile.find(LIB_PATH);

	if (libPathPos != std::string::npos)
	{
		pathWithOutFile.erase(libPathPos, std::string(LIB_PATH).length());
		pathWithOutFile.pop_back();
	}

	return GetTypeOfName(pathWithOutFile);
}

ResourceType ModuleResources::FindTypeByExtension(const std::string& path)
{
	std::string fileExtension = App->GetModule<ModuleFileSystem>()->GetFileExtension(path);
	std::string normalizedExtension = "";

	for (int i = 0; i < fileExtension.size(); ++i)
	{
		normalizedExtension.push_back(tolower(fileExtension[i]));
	}

	if (normalizedExtension == MODEL_EXTENSION)
	{
		return ResourceType::Model;
	}
	else if (normalizedExtension == JPG_TEXTURE_EXTENSION || normalizedExtension == PNG_TEXTURE_EXTENSION ||
			 normalizedExtension == TIF_TEXTURE_EXTENSION || normalizedExtension == DDS_TEXTURE_EXTENSION ||
			 normalizedExtension == TGA_TEXTURE_EXTENSION || normalizedExtension == HDR_TEXTURE_EXTENSION)
	{
		return ResourceType::Texture;
	}
	else if (normalizedExtension == SKYBOX_EXTENSION)
	{
		return ResourceType::SkyBox;
	}
	else if (normalizedExtension == CUBEMAP_EXTENSION)
	{
		return ResourceType::Cubemap;
	}
	else if (normalizedExtension == SCENE_EXTENSION)
	{
		return ResourceType::Scene;
	}
	else if (normalizedExtension == MATERIAL_EXTENSION)
	{
		return ResourceType::Material;
	}
	else if (normalizedExtension == NAVMESH_EXTENSION)
	{
		return ResourceType::NavMesh;
	}
	else if (normalizedExtension == MESH_EXTENSION)
	{
		return ResourceType::Mesh;
	}
	else if (normalizedExtension == ANIMATION_EXTENSION)
	{
		return ResourceType::Animation;
	}
	else if (normalizedExtension == STATEMACHINE_EXTENSION)
	{
		return ResourceType::StateMachine;
	}
	else if (normalizedExtension == PARTICLESYSTEM_EXTENSION)
	{
		return ResourceType::ParticleSystem;
	}

	return ResourceType::Unknown;
}

const std::string ModuleResources::GetNameOfType(ResourceType type)
{
	switch (type)
	{
		case ResourceType::Model:
			return "Models";
		case ResourceType::Texture:
			return "Textures";
		case ResourceType::Mesh:
			return "Meshes";
		case ResourceType::Scene:
			return "Scenes";
		case ResourceType::Material:
			return "Materials";
		case ResourceType::NavMesh:
			return "NavMesh";
		case ResourceType::SkyBox:
			return "SkyBox";
		case ResourceType::Cubemap:
			return "Cubemaps";
		case ResourceType::Animation:
			return "Animation";
		case ResourceType::StateMachine:
			return "StateMachine";
		case ResourceType::ParticleSystem:
			return "ParticleSystem";
		case ResourceType::Unknown:
		default:
			return "Unknown";
	}
}

ResourceType ModuleResources::GetTypeOfName(const std::string& typeName)
{
	if (typeName == "Models")
	{
		return ResourceType::Model;
	}
	if (typeName == "Textures")
	{
		return ResourceType::Texture;
	}
	if (typeName == "Meshes")
	{
		return ResourceType::Mesh;
	}
	if (typeName == "Scenes")
	{
		return ResourceType::Scene;
	}
	if (typeName == "Materials")
	{
		return ResourceType::Material;
	}
	if (typeName == "NavMesh")
	{
		return ResourceType::NavMesh;
	}
	if (typeName == "SkyBox")
	{
		return ResourceType::SkyBox;
	}
	if (typeName == "Cubemaps")
	{
		return ResourceType::Cubemap;
	}
	if (typeName == "Animation")
	{
		return ResourceType::Animation;
	}
	if (typeName == "StateMachine")
	{
		return ResourceType::StateMachine;
	}
	if (typeName == "ParticleSystem")
	{
		return ResourceType::ParticleSystem;
	}
	return ResourceType::Unknown;
}

const std::string ModuleResources::GetFolderOfType(ResourceType type)
{
	return GetNameOfType(type) + "/";
}

const std::string ModuleResources::CreateAssetsPath(const std::string& fileName, ResourceType type)
{
	std::string assetsPath = assetsFolder;
	assetsPath += GetFolderOfType(type);
	assetsPath += fileName;
	return assetsPath;
}

const std::string ModuleResources::CreateLibraryPath(UID resourceUID, ResourceType type)
{
	std::string libraryPath = libraryFolder;
	libraryPath += GetFolderOfType(type);
	libraryPath += std::to_string(resourceUID);
	return libraryPath;
}
