#include "ModuleResources.h"

#include "Application.h"
#include "ModuleFileSystem.h"

#include "FileSystem/Importers/AnimationImporter.h"
#include "FileSystem/Importers/CubemapImporter.h"
#include "FileSystem/Importers/MaterialImporter.h"
#include "FileSystem/Importers/MeshImporter.h"
#include "FileSystem/Importers/ModelImporter.h"
#include "FileSystem/Importers/SkyBoxImporter.h"
#include "FileSystem/Importers/StateMachineImporter.h"
#include "FileSystem/Importers/TextureImporter.h"

#include "Resources/EditorResource/EditorResource.h"
#include "Resources/ResourceAnimation.h"
#include "Resources/ResourceCubemap.h"
#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceSkyBox.h"
#include "Resources/ResourceTexture.h"

#include "Auxiliar/CollectionAwareDeleter.h"

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
	materialImporter = std::make_unique<MaterialImporter>();
	skyboxImporter = std::make_unique<SkyBoxImporter>();
	cubemapImporter = std::make_unique<CubemapImporter>();
	animationImporter = std::make_unique<AnimationImporter>();
	stateMachineImporter = std::make_unique<StateMachineImporter>();

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
	//std::shared_ptr<Resource> importedRes;
	std::string assetsPath = CreateAssetsPath(fileName, type);
	switch (type)
	{
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
#ifdef ENGINE
	std::shared_ptr<EditorResourceInterface> res = nullptr;
	switch (type)
	{
		case ResourceType::Model:
			res = std::shared_ptr<EditorResource<ResourceModel>>(
				new EditorResource<ResourceModel>(uid, fileName, assetsPath, libraryPath),
				CollectionAwareDeleter<Resource>());
			break;
		case ResourceType::Texture:
			res = std::shared_ptr<EditorResource<ResourceTexture>>(
				new EditorResource<ResourceTexture>(uid, fileName, assetsPath, libraryPath),
				CollectionAwareDeleter<Resource>());
			break;
		case ResourceType::Mesh:
			res = std::shared_ptr<EditorResource<ResourceMesh>>(
				new EditorResource<ResourceMesh>(uid, fileName, assetsPath, libraryPath),
				CollectionAwareDeleter<Resource>());
			break;
		case ResourceType::Scene: // TODO
			return nullptr;
		case ResourceType::Material:
			res = std::shared_ptr<EditorResource<ResourceMaterial>>(
				new EditorResource<ResourceMaterial>(uid, fileName, assetsPath, libraryPath),
				CollectionAwareDeleter<Resource>());
			break;
		case ResourceType::SkyBox:
			res = std::shared_ptr<EditorResource<ResourceSkyBox>>(
				new EditorResource<ResourceSkyBox>(uid, fileName, assetsPath, libraryPath),
				CollectionAwareDeleter<Resource>());
			break;
		case ResourceType::Cubemap:
			res = std::shared_ptr<EditorResource<ResourceCubemap>>(
				new EditorResource<ResourceCubemap>(uid, fileName, assetsPath, libraryPath),
				CollectionAwareDeleter<Resource>());
			break;
		case ResourceType::Animation:
			res = std::shared_ptr<EditorResource<ResourceAnimation>>(
				new EditorResource<ResourceAnimation>(uid, fileName, assetsPath, libraryPath),
				CollectionAwareDeleter<Resource>());
			break;
		case ResourceType::StateMachine:
			res = std::shared_ptr<EditorResource<ResourceStateMachine>>(
				new EditorResource<ResourceStateMachine>(uid, fileName, assetsPath, libraryPath),
				CollectionAwareDeleter<Resource>());
			break;
		default:
			return nullptr;
	}

	std::get_deleter<CollectionAwareDeleter<Resource>>(res)->AddCollection(resources);
	return res;
#else
	auto customDeleter = [this](Resource* pointer)
	{
		std::map<UID, std::weak_ptr<Resource>>& map = resources;
		for (auto it = std::begin(map); it != std::end(map);)
		{
			if (it->second.expired())
			{
				it = map.erase(it);
			}
			else
			{
				++it;
			}
		}
		delete pointer;
	};
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
		case ResourceType::Scene: // TODO
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
	if (resource->GetType() == ResourceType::Material)
	{
		std::shared_ptr<ResourceMaterial> materialResource = std::dynamic_pointer_cast<ResourceMaterial>(resource);
		ReImportMaterialAsset(materialResource);
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
	std::vector<ResourceType> allResourceTypes = { ResourceType::Material,	  ResourceType::Mesh,
												   ResourceType::Model,		  ResourceType::Scene,
												   ResourceType::Texture,	  ResourceType::SkyBox,
												   ResourceType::Cubemap,	  ResourceType::Animation,
												   ResourceType::StateMachine };

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
		for (auto resourceit = resources.begin(); resourceit != resources.end();)
		{
			const std::shared_ptr<Resource>& resource = resourceit->second.lock();
			if (resource)
			{
				if (resource->GetType() != ResourceType::Mesh && !fileSystem->Exists(resource->GetAssetsPath().c_str()))
				{
					toRemove.push_back(std::dynamic_pointer_cast<EditorResourceInterface>(resource));
				}
				else
				{
					std::string libraryPathWithExtension = fileSystem->GetPathWithExtension(resource->GetLibraryPath());

					if (libraryPathWithExtension.empty() /*file with that name was not found*/ ||
						!fileSystem->Exists(libraryPathWithExtension.c_str()) || resource->IsChanged())
					{
						toCreateLib.push_back(resource);
						resource->SetChanged(false);
					}
					if (!fileSystem->Exists((resource->GetAssetsPath() + META_EXTENSION).c_str()))
					{
						toCreateMeta.push_back(resource);
					}
					// these type's assets are binary files changed in runtime
					else if (resource->GetType() != ResourceType::Mesh && resource->GetType() != ResourceType::Material)
					{
						long long assetTime = fileSystem->GetModificationDate(resource->GetAssetsPath().c_str());
						long long libTime = fileSystem->GetModificationDate(
							(resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str());
						if (assetTime > libTime)
						{
							toImport.push_back(resource);
						}
					}
				}
			}

			if (resourceit != resources.end())
			{
				++resourceit;
			}
		}
		// Remove resources
		for (std::shared_ptr<EditorResourceInterface> resource : toRemove)
		{
			DeleteResource(resource);
		}
		// Import resources
		for (std::shared_ptr<Resource>& resource : toImport)
		{
			AddResource(resource, resource->GetAssetsPath());
		}
		for (std::shared_ptr<Resource>& resource : toCreateLib)
		{
			if (resource->GetType() == ResourceType::Material)
			{
				std::shared_ptr<ResourceMaterial> materialResource =
					std::dynamic_pointer_cast<ResourceMaterial>(resource);
				ReImportMaterialAsset(materialResource);
			}
			else if (resource->GetType() == ResourceType::StateMachine)
			{
				std::shared_ptr<ResourceStateMachine> stateMachineResource =
					std::dynamic_pointer_cast<ResourceStateMachine>(resource);
				char* saveBuffer = {};
				unsigned int size = 0;
				stateMachineImporter->Save(stateMachineResource, saveBuffer, size);
				App->GetModule<ModuleFileSystem>()->Save(
					(resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), saveBuffer, size);
				delete saveBuffer;
				break;
			}
			ImportResourceFromSystem(resource->GetAssetsPath(), resource, resource->GetType());
		}
		for (std::shared_ptr<Resource>& resource : toCreateMeta)
		{
			CreateMetaFileOfResource(resource);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(4000));
	}
}

void ModuleResources::ReImportMaterialAsset(const std::shared_ptr<ResourceMaterial>& materialResource)
{
	std::vector<std::string> pathTextures;

	std::shared_ptr<ResourceTexture> textureDiffuse = materialResource->GetDiffuse();
	textureDiffuse ? pathTextures.push_back(textureDiffuse->GetAssetsPath()) : pathTextures.push_back(std::string());

	std::shared_ptr<ResourceTexture> textureNormal = materialResource->GetNormal();
	textureNormal ? pathTextures.push_back(textureNormal->GetAssetsPath()) : pathTextures.push_back(std::string());

	std::shared_ptr<ResourceTexture> textureOcclusion = materialResource->GetOcclusion();
	textureOcclusion ? pathTextures.push_back(textureOcclusion->GetAssetsPath())
					 : pathTextures.push_back(std::string());

	/*std::shared_ptr<ResourceTexture> textureSpecular = materialResource->GetSpecular();
	textureSpecular ? pathTextures.push_back(textureSpecular->GetAssetsPath()) : pathTextures.push_back("");*/

	std::shared_ptr<ResourceTexture> textureMetallic = materialResource->GetMetallic();
	textureMetallic ? pathTextures.push_back(textureMetallic->GetAssetsPath()) : pathTextures.push_back(std::string());

	char* fileBuffer{};
	unsigned int size = 0;
	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();

	fileSystem->SaveInfoMaterial(pathTextures, fileBuffer, size);
	std::string materialPath = materialResource->GetAssetsPath();

	std::string metaPath = materialResource->GetAssetsPath() + META_EXTENSION;
	char* metaBuffer = {};
	fileSystem->Load(metaPath.c_str(), metaBuffer);
	rapidjson::Document doc;
	Json meta(doc, doc);
	meta.fromBuffer(metaBuffer);
	delete metaBuffer;

	meta["DiffuseAssetPath"] =
		materialResource->GetDiffuse() ? materialResource->GetDiffuse()->GetAssetsPath().c_str() : "";
	meta["NormalAssetPath"] =
		materialResource->GetNormal() ? materialResource->GetNormal()->GetAssetsPath().c_str() : "";
	meta["OcclusionAssetPath"] =
		materialResource->GetOcclusion() ? materialResource->GetOcclusion()->GetAssetsPath().c_str() : "";
	meta["SpecularAssetPath"] =
		materialResource->GetSpecular() ? materialResource->GetSpecular()->GetAssetsPath().c_str() : "";
	meta["MetallicAssetPath"] =
		materialResource->GetMetallic() ? materialResource->GetMetallic()->GetAssetsPath().c_str() : "";

	rapidjson::StringBuffer buffer;
	meta.toBuffer(buffer);
	fileSystem->Save(metaPath.c_str(), buffer.GetString(), (unsigned int) buffer.GetSize());
	fileSystem->Save(materialPath.c_str(), fileBuffer, size);
	delete fileBuffer;
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
		case ResourceType::SkyBox:
			return "SkyBox";
		case ResourceType::Cubemap:
			return "Cubemaps";
		case ResourceType::Animation:
			return "Animation";
		case ResourceType::StateMachine:
			return "StateMachine";
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
