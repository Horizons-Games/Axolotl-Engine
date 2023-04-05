#include "ModuleResources.h"

#include "Application.h"
#include "ModuleFileSystem.h"

#include "FileSystem/Importers/ModelImporter.h"
#include "FileSystem/Importers/MeshImporter.h"
#include "FileSystem/Importers/TextureImporter.h"
#include "FileSystem/Importers/MaterialImporter.h"
#include "FileSystem/Importers/SkyBoxImporter.h"
#include "FileSystem/Importers/AnimationImporter.h"

#include "Resources/EditorResource/EditorResource.h"
#include "Resources/ResourceSkyBox.h"
#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceTexture.h"

#include "Auxiliar/CollectionAwareDeleter.h"

#include <future>

const std::string ModuleResources::assetsFolder = "Assets/";
const std::string ModuleResources::libraryFolder = "Lib/";

//creator and destructor can't be inlined
//because we are using unique pointers with forward declarations
ModuleResources::ModuleResources() : monitorResources (false)
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
	animationImporter = std::make_unique<AnimationImporter>();

	CreateAssetAndLibFolders();

#ifdef ENGINE
	monitorThread = std::thread(&ModuleResources::MonitorResources, this);
#endif
	return true;
}

bool ModuleResources::CleanUp()
{
#ifdef ENGINE
	monitorResources = false;
	monitorThread.join();
#endif
	resources.clear();
	return true;
}

//Creates Binary and Meta from an Asset original path
std::shared_ptr<Resource> ModuleResources::ImportResource(const std::string& originalPath)
{
	ResourceType type = FindTypeByExtension(originalPath);
	if (type == ResourceType::Unknown)
	{
		ENGINE_LOG("Extension not supported");
	}
	std::string fileName = App->fileSystem->GetFileName(originalPath);
	std::string extension = App->fileSystem->GetFileExtension(originalPath);
	std::string assetsPath = originalPath;

	assetsPath = CreateAssetsPath(fileName + extension, type);

	bool resourceExists = App->fileSystem->Exists(assetsPath.c_str());
	if (!resourceExists)
		App->fileSystem->CopyFileInAssets(originalPath, assetsPath);

	std::shared_ptr<Resource> importedRes = CreateNewResource(fileName, assetsPath, type);
	CreateMetaFileOfResource(importedRes);
	ImportResourceFromSystem(originalPath, importedRes, importedRes->GetType());
	return importedRes;
}

std::shared_ptr<Resource> ModuleResources::ImportThread(const std::string& originalPath)
{
	std::promise<std::shared_ptr<Resource>> p;
	std::future<std::shared_ptr<Resource>> f = p.get_future();
	std::thread importThread = std::thread(
		[&]() 
		{
			p.set_value(ImportResource(originalPath));
		}
	);
	importThread.detach();
	return f.get();
}

std::shared_ptr<Resource> ModuleResources::CreateNewResource(const std::string& fileName,
															 const std::string& assetsPath,
															 ResourceType type)
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
		res = std::shared_ptr<EditorResource<ResourceModel>>(new EditorResource<ResourceModel>(uid, fileName, assetsPath, libraryPath), CollectionAwareDeleter<Resource>());
		break;
	case ResourceType::Texture:
		res = std::shared_ptr<EditorResource<ResourceTexture>>(new EditorResource<ResourceTexture>(uid, fileName, assetsPath, libraryPath), CollectionAwareDeleter<Resource>());
		break;
	case ResourceType::Mesh:
		res = std::shared_ptr<EditorResource<ResourceMesh>>(new EditorResource<ResourceMesh>(uid, fileName, assetsPath, libraryPath), CollectionAwareDeleter<Resource>());
		break;
	case ResourceType::Scene: //TODO
		return nullptr;
	case ResourceType::Material:
		res = std::shared_ptr<EditorResource<ResourceMaterial>>(new EditorResource<ResourceMaterial>(uid, fileName, assetsPath, libraryPath), CollectionAwareDeleter<Resource>());
		break;
	case ResourceType::SkyBox:
		res = std::shared_ptr<EditorResource<ResourceSkyBox>>(new EditorResource<ResourceSkyBox>(uid, fileName, assetsPath, libraryPath), CollectionAwareDeleter<Resource>());
		break;
	default:
		return nullptr;
	}

	std::get_deleter<CollectionAwareDeleter<Resource>>(res)->AddCollection(resources);
	return res;
#else
	switch (type)
	{
	case ResourceType::Model:
		return std::make_shared<ResourceModel>(uid, fileName, assetsPath, libraryPath);
	case ResourceType::Texture:
		return std::make_shared<ResourceTexture>(uid, fileName, assetsPath, libraryPath);
	case ResourceType::Mesh:
		return std::make_shared<ResourceMesh>(uid, fileName, assetsPath, libraryPath);
	case ResourceType::Scene: //TODO
		return nullptr;
	case ResourceType::Material:
		return std::make_shared<ResourceMaterial>(uid, fileName, assetsPath, libraryPath);
	case ResourceType::SkyBox:
		return std::make_shared<ResourceSkyBox>(uid, fileName, assetsPath, libraryPath);
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
	//resToDelete->MarkToDelete();

	//std::string libPath = resToDelete->GetLibraryPath() + GENERAL_BINARY_EXTENSION;
	//std::string metaPath = resToDelete->GetLibraryPath() + META_EXTENSION;
	//App->fileSystem->Delete(metaPath.c_str());
	//App->fileSystem->Delete(libPath.c_str());

	//if (resToDelete)
	//{
	//	if (resToDelete->GetType() == ResourceType::Model)
	//	{
	//		std::shared_ptr<ResourceModel> modelToDelete = std::dynamic_pointer_cast<ResourceModel>(resToDelete);
	//		for (const std::shared_ptr<ResourceMesh>& mesh : modelToDelete->GetMeshes())
	//		{
	//			DeleteResource(std::dynamic_pointer_cast<EditorResourceInterface>(mesh));
	//		}
	//	}
	//	else if (resToDelete->GetType() == ResourceType::Mesh) //mesh should not have an asset
	//	{
	//		App->fileSystem->Delete(resToDelete->GetAssetsPath().c_str());
	//	}
	//}

	//resources.erase(resToDelete->GetUID());
}

std::shared_ptr<Resource> ModuleResources::LoadResourceStored(const char* filePath, const char* fileNameToStore)
{
	std::vector<std::string> files = App->fileSystem->ListFiles(filePath);
	for (size_t i = 0; i < files.size(); i++)
	{
		std::string path (filePath);
		path += '/' +  files[i];
		const char* file = path.c_str();
		if (App->fileSystem->IsDirectory(file))
		{
			std::shared_ptr<Resource> resource = LoadResourceStored(file, fileNameToStore);
			if (resource) return resource;
		}
		else 
		{
			if (App->fileSystem->GetFileName(file) == fileNameToStore)
			{
				std::string fileName = App->fileSystem->GetFileName(file);
				UID uid = std::stoull(fileName.c_str(), NULL, 0);
				ResourceType type = FindTypeByFolder(file);
				std::shared_ptr<Resource> resource = CreateResourceOfType(uid, fileName, "", App->fileSystem->GetPathWithoutExtension(file), type);

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

	if (resource->GetType() != ResourceType::Unknown && App->fileSystem->Exists(libPath.c_str()))
	{
		if (resource != nullptr)
		{
			char* binaryBuffer = {};
			App->fileSystem->Load(libPath.c_str(), binaryBuffer);

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
			case ResourceType::Animation:
				animationImporter->Load(binaryBuffer, std::dynamic_pointer_cast<ResourceAnimation>(resource));
				break;
			default:
				break;
			}
			return;
		}
	}
}

void ModuleResources::ReimportResource(UID resourceUID)
{
	std::shared_ptr<Resource> resource = resources[resourceUID].lock();
	CreateMetaFileOfResource(resource);
	ImportResourceFromSystem(resource->GetAssetsPath(), resource, resource->GetType());
}

void ModuleResources::CreateMetaFileOfResource(std::shared_ptr<Resource>& resource)
{
	std::string metaPath = resource->GetAssetsPath() + META_EXTENSION;
	rapidjson::Document doc;
	Json meta(doc, doc);

	if (!resource->IsChanged() && App->fileSystem->Exists(metaPath.c_str()))
	{
		char* metaBuffer = {};
		App->fileSystem->Load(metaPath.c_str(), metaBuffer);
		meta.fromBuffer(metaBuffer);
		delete metaBuffer;
		resource = CreateResourceOfType(meta["UID"],resource->GetFileName(),resource->GetAssetsPath(),
			CreateLibraryPath(meta["UID"],resource->GetType()),resource->GetType());
		resource->LoadImporterOptions(meta);
	}
	else
	{
		meta["UID"] = resource->GetUID();
		meta["Type"] = GetNameOfType(resource->GetType()).c_str();
		resource->SaveImporterOptions(meta);
		rapidjson::StringBuffer buffer;
		meta.toBuffer(buffer);
		App->fileSystem->Save(metaPath.c_str(), buffer.GetString(), (unsigned int)buffer.GetSize());
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
		skyboxImporter->Import(originalPath.c_str(),
			std::dynamic_pointer_cast<ResourceSkyBox>(resource));
		break;
	case ResourceType::Animation:
		animationImporter->Import(originalPath.c_str(),
			std::dynamic_pointer_cast<ResourceAnimation>(resource));
		break;
	default:
		break;
	}
}

void ModuleResources::CreateAssetAndLibFolders()
{
	bool assetsFolderNotCreated = !App->fileSystem->Exists(ASSETS_PATH);
	if (assetsFolderNotCreated)
	{
		App->fileSystem->CreateDirectoryA(ASSETS_PATH);
	}
	bool libraryFolderNotCreated = !App->fileSystem->Exists(LIB_PATH);
	if (libraryFolderNotCreated)
	{
		App->fileSystem->CreateDirectoryA(LIB_PATH);
	}
	//seems there is no easy way to iterate over enum classes in C++ :/
	//(actually there is a library that looks really clean but might be overkill:
	// https://github.com/Neargye/magic_enum)
	//ensure this vector is updated whenever a new type of resource is added
	std::vector<ResourceType> allResourceTypes = { ResourceType::Material,
												  ResourceType::Mesh,
												  ResourceType::Model,
												  ResourceType::Scene,
												  ResourceType::Texture,
												  ResourceType::SkyBox,
												  ResourceType::Animation};
	for (ResourceType type : allResourceTypes)
	{
		std::string folderOfType = GetFolderOfType(type);

		std::string assetsFolderOfType = ASSETS_PATH + folderOfType;
		bool assetsFolderOfTypeNotCreated = !App->fileSystem->Exists(assetsFolderOfType.c_str());
		if (assetsFolderOfTypeNotCreated)
		{
			App->fileSystem->CreateDirectoryA(assetsFolderOfType.c_str());
		}

		std::string libraryFolderOfType = LIB_PATH + folderOfType;
		bool libraryFolderOfTypeNotCreated = !App->fileSystem->Exists(libraryFolderOfType.c_str());
		if (libraryFolderOfTypeNotCreated)
		{
			App->fileSystem->CreateDirectoryA(libraryFolderOfType.c_str());
		}
	}
}

void ModuleResources::MonitorResources()
{
	while (monitorResources) 
	{
		CreateAssetAndLibFolders();
		std::vector<std::shared_ptr<EditorResourceInterface>> toRemove;
		std::vector<std::shared_ptr<Resource>> toImport;
		std::vector<std::shared_ptr<Resource>> toCreateLib;
		std::vector<std::shared_ptr<Resource>> toCreateMeta;
		std::map<UID, std::weak_ptr<Resource> >::iterator it;
		for (it = resources.begin(); it != resources.end(); ++it)
		{
			const std::shared_ptr<Resource>& resource = it->second.lock();
			if (resource)
			{
				if (resource->GetType() != ResourceType::Mesh &&
					!App->fileSystem->Exists(resource->GetAssetsPath().c_str()))
				{
					toRemove.push_back(std::dynamic_pointer_cast<EditorResourceInterface>(resource));
				}
				else
				{
					std::string libraryPathWithExtension =
						App->fileSystem->GetPathWithExtension(resource->GetLibraryPath());

					if (libraryPathWithExtension == "" /*file with that name was not found*/ ||
						!App->fileSystem->Exists(libraryPathWithExtension.c_str()) ||
						resource->IsChanged())
					{
						toCreateLib.push_back(resource);
						resource->SetChanged(false);
					}
					if (!App->fileSystem->Exists((resource->GetAssetsPath() + META_EXTENSION).c_str()))
					{
						toCreateMeta.push_back(resource);
					}
					//these type's assets are binary files changed in runtime
					else if (resource->GetType() != ResourceType::Mesh &&
						resource->GetType() != ResourceType::Material)
					{
						long long assetTime =
							App->fileSystem->GetModificationDate(resource->GetAssetsPath().c_str());
						long long libTime =
							App->fileSystem->GetModificationDate((resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str());
						if (assetTime > libTime)
						{
							toImport.push_back(resource);
						}
					}
				}
			}
		}
		//Remove resources
		for (std::shared_ptr<EditorResourceInterface> resource : toRemove)
		{
			DeleteResource(resource);
		}
		//Import resources
		for (std::shared_ptr<Resource>& resource : toImport)
		{
			AddResource(resource, resource->GetAssetsPath());
		}
		for (std::shared_ptr<Resource>& resource : toCreateLib)
		{
			if (resource->GetType() == ResourceType::Material) 
			{
				std::shared_ptr<ResourceMaterial> materialResource = std::dynamic_pointer_cast<ResourceMaterial>(resource);
				ReImportMaterialAsset(materialResource);
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
	textureDiffuse ? pathTextures.push_back(textureDiffuse->GetAssetsPath()) : pathTextures.push_back("");

	std::shared_ptr<ResourceTexture> textureNormal = materialResource->GetNormal();
	textureNormal ? pathTextures.push_back(textureNormal->GetAssetsPath()) : pathTextures.push_back("");

	std::shared_ptr<ResourceTexture> textureOcclusion = materialResource->GetOcclusion();
	textureOcclusion ? pathTextures.push_back(textureOcclusion->GetAssetsPath()) : pathTextures.push_back("");

	std::shared_ptr<ResourceTexture> textureSpecular = materialResource->GetSpecular();
	textureSpecular ? pathTextures.push_back(textureSpecular->GetAssetsPath()) : pathTextures.push_back("");

	char* fileBuffer{};
	unsigned int size = 0;

	App->fileSystem->SaveInfoMaterial(pathTextures, fileBuffer, size);
	std::string materialPath = materialResource->GetAssetsPath();

	App->fileSystem->Save(materialPath.c_str(), fileBuffer, size);
	delete fileBuffer;
}

bool ModuleResources::ExistsResourceWithAssetsPath(const std::string& assetsPath, UID& resourceUID)
{
	std::map<UID, std::weak_ptr<Resource> >::iterator it;
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
	std::string pathWithOutFile = App->fileSystem->GetPathWithoutFile(path);
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
	std::string fileExtension = App->fileSystem->GetFileExtension(path);
	std::string normalizedExtension = "";

	for(int i = 0; i < fileExtension.size(); ++i) 
	{
		normalizedExtension.push_back(tolower(fileExtension[i]));
	}

	if (normalizedExtension == MODEL_EXTENSION)
	{
		return ResourceType::Model;
	}
	else if (normalizedExtension == JPG_TEXTURE_EXTENSION || normalizedExtension == PNG_TEXTURE_EXTENSION || 
			normalizedExtension == TIF_TEXTURE_EXTENSION || normalizedExtension == DDS_TEXTURE_EXTENSION || 
			normalizedExtension == TGA_TEXTURE_EXTENSION) 
	{
		return ResourceType::Texture;
	}
	else if(normalizedExtension == SKYBOX_EXTENSION)
	{
		return ResourceType::SkyBox;
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
	case ResourceType::Animation:
		return "Animation";
	case ResourceType::Unknown:
	default:
		return "Unknown";
	}
}

ResourceType ModuleResources::GetTypeOfName(const std::string& typeName)
{
	if (typeName == "Models")
		return ResourceType::Model;
	if (typeName == "Textures")
		return ResourceType::Texture;
	if (typeName == "Meshes")
		return ResourceType::Mesh;
	if (typeName == "Scenes")
		return ResourceType::Scene;
	if (typeName == "Materials")
		return ResourceType::Material;
	if (typeName == "SkyBox")
		return ResourceType::SkyBox;
	if (typeName == "Animation")
		return ResourceType::Animation;
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
