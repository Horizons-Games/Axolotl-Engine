#include "ModuleResources.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "Json.h"

#include "FileSystem/Importers/Importer.h"
#include "FileSystem/Importers/ModelImporter.h"
#include "FileSystem/Importers/MeshImporter.h"
#include "FileSystem/Importers/TextureImporter.h"
#include "FileSystem/Importers/MaterialImporter.h"

#include "Resources/Resource.h"
#include "Resources/ResourceMesh.h"
#include "Resources/ResourceTexture.h"
#include <thread>
#include <future>

const std::string ModuleResources::assetsFolder = "Assets/";
const std::string ModuleResources::libraryFolder = "Lib/";

void  ModuleResources::MonitorResources()
{
	while (monitorResources) 
	{
		std::vector<UID> toRemove;
		std::vector<std::shared_ptr<Resource> > toImport;
		std::vector<std::shared_ptr<Resource> > toCreateLib;
		std::vector<std::shared_ptr<Resource> > toCreateMeta;
		std::map<UID, std::shared_ptr<Resource> >::iterator it;
		for (it = resources.begin(); it != resources.end(); ++it)
		{
			if (it->second->GetType() != ResourceType::Mesh && !App->fileSystem->Exists(it->second->GetAssetsPath().c_str()))
			{
				toRemove.push_back(it->first);
			}
			else 
			{
				if (!App->fileSystem->Exists(it->second->GetLibraryPath().c_str()))
				{
					toCreateLib.push_back(it->second);
				}
				if (!App->fileSystem->Exists((it->second->GetLibraryPath() + META_EXTENSION).c_str()))
				{
					toCreateMeta.push_back(it->second);
				}
				else
				{
					long long assetTime =
						App->fileSystem->GetModificationDate(it->second->GetAssetsPath().c_str());
					long long libTime =
						App->fileSystem->GetModificationDate((it->second->GetLibraryPath() + META_EXTENSION).c_str());
					if (assetTime > libTime)
					{
						toImport.push_back(it->second);
					}
				}
			}
		}
		//Remove resources
		for (UID resUID : toRemove)
		{
			std::string libPath = resources[resUID]->GetAssetsPath();
			std::string metaPath = resources[resUID]->GetAssetsPath() + META_EXTENSION;
			App->fileSystem->Delete(metaPath.c_str());
			App->fileSystem->Delete(libPath.c_str());
			resources.erase(resUID);

		}
		//Import resources
		for (std::shared_ptr<Resource> resource : toImport)
		{
			AddResource(resource, resource->GetAssetsPath());
		}
		for (std::shared_ptr<Resource> resource : toCreateLib)
		{
			ImportResourceFromSystem(resource->GetAssetsPath(), resource, resource->GetType());
		}
		for (std::shared_ptr<Resource> resource : toCreateMeta)
		{
			CreateMetaFileOfResource(resource);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(4000));
	}
	
}

void ModuleResources::LoadResourceStored(const char* filePath)
{
	std::vector<std::string> files = App->fileSystem->listFiles(filePath);
	for (size_t i = 0; i < files.size(); i++)
	{
		std::string path (filePath);
		path += '/' +  files[i];
		const char* file = path.c_str();
		if (App->fileSystem->IsDirectory(file))
		{
			LoadResourceStored(file);
		}
		else 
		{
			if (GetFileExtension(path) != ".meta")
			{
				ImportResourceFromLibrary(file);
			}
		}
	}
}

void ModuleResources::ImportResourceFromLibrary(const std::string& libraryPath)
{
	std::string metaPath;
	std::string fileExtension = GetFileExtension(libraryPath);
	int posOfExtensionInPath = libraryPath.find(fileExtension);
	if (posOfExtensionInPath != 0) //has file extension
	{
		std::string libraryPathCopy = std::string(libraryPath);
		metaPath = libraryPathCopy.erase(posOfExtensionInPath, fileExtension.size());
	}
	else
	{
		metaPath = libraryPath;
	}
	metaPath += META_EXTENSION;
	
	if (App->fileSystem->Exists(metaPath.c_str())){
		char* metaBuffer = {};
		App->fileSystem->Load(metaPath.c_str(), metaBuffer);

		rapidjson::Document doc;
		Json Json(doc, doc);

		Json.fromBuffer(metaBuffer);

		UID uid = (UID)Json["UID"];
		ResourceType type = GetTypeOfName(std::string(Json["Type"]));

		if (type != ResourceType::Unknown)
		{
			std::string fileName = GetFileName(libraryPath);
			std::string assetsPath = CreateAssetsPath(fileName, type);
			std::shared_ptr<Resource> resource = CreateResourceOfType(uid, fileName, assetsPath, libraryPath, type);
			
			if (resource != nullptr)
			{
				char* binaryBuffer = {};
				App->fileSystem->Load(libraryPath.c_str(), binaryBuffer);

				switch (type)
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
					break;
				default:
					break;
				}

				resources.insert({ uid, resource });
			}
		}
	}
}

void ModuleResources::AddResource(std::shared_ptr<Resource>& resource, const std::string& originalPath)
{
	CreateMetaFileOfResource(resource);

	ImportResourceFromSystem(originalPath, resource, resource->GetType());

	UID uid = resource->GetUID();
	resources.insert({ uid, resource });
}

bool ModuleResources::Start()
{
	monitorResources = true;
	modelImporter = std::make_shared<ModelImporter>();
	textureImporter = std::make_shared<TextureImporter>();
	meshImporter = std::make_shared<MeshImporter>();
	materialImporter = std::make_shared<MaterialImporter>();

	bool assetsFolderNotCreated = !App->fileSystem->Exists(assetsFolder.c_str());
	if (assetsFolderNotCreated)
	{
		App->fileSystem->CreateDirectoryA(assetsFolder.c_str());
	}
	bool libraryFolderNotCreated = !App->fileSystem->Exists(libraryFolder.c_str());
	if (libraryFolderNotCreated)
	{
		App->fileSystem->CreateDirectoryA(libraryFolder.c_str());
	}
	//seems there is no easy way to iterate over enum classes in C++ :/
	//(actually there is a library that looks really clean but might be overkill:
	// https://github.com/Neargye/magic_enum)
	//ensure this vector is updated whenever a new type of resource is added
	std::vector<ResourceType> allResourceTypes = {ResourceType::Material,
												  ResourceType::Mesh,
												  ResourceType::Model,
												  ResourceType::Scene,
												  ResourceType::Texture,
												  ResourceType::SkyBox};
	for (ResourceType type : allResourceTypes)
	{
		std::string folderOfType = GetFolderOfType(type);

		std::string assetsFolderOfType = assetsFolder + folderOfType;
		bool assetsFolderOfTypeNotCreated = !App->fileSystem->Exists(assetsFolderOfType.c_str());
		if (assetsFolderOfTypeNotCreated)
		{
			App->fileSystem->CreateDirectoryA(assetsFolderOfType.c_str());
		}

		std::string libraryFolderOfType = libraryFolder + folderOfType;
		bool libraryFolderOfTypeNotCreated = !App->fileSystem->Exists(libraryFolderOfType.c_str());
		if (libraryFolderOfTypeNotCreated)
		{
			App->fileSystem->CreateDirectoryA(libraryFolderOfType.c_str());
		}
	}
	//remove file separator from library folder
	LoadResourceStored(libraryFolder.substr(0, libraryFolder.length() - 1).c_str());
	//monitorThread = std::thread(&ModuleResources::MonitorResources, this);
	return true;
}

UID ModuleResources::ImportThread(const std::string& originalPath)
{
	std::promise<UID> p;
	std::future<UID> f = p.get_future();
	std::thread importThread = std::thread(
		[&]() 
		{
			p.set_value(ImportResource(originalPath));
		}
	);
	importThread.detach();
	return f.get();
}

UID ModuleResources::ImportResource(const std::string& originalPath)
{
	ResourceType type = FindTypeByPath(originalPath);
	if (type == ResourceType::Unknown)
	{
		ENGINE_LOG("Extension not supported");
		return 0;
	}
	std::string fileName = GetFileName(originalPath);
	std::string extension = GetFileExtension(originalPath);
	std::string assetsPath = originalPath;

	if (type != ResourceType::Mesh && type != ResourceType::Material) 
	{
		//is the extension necessary?
		//if so, we need a way to find the asset path (name + etension)
		//given the path of its binary
		assetsPath = CreateAssetsPath(fileName + extension, type);

		bool resourceExists = App->fileSystem->Exists(assetsPath.c_str());
		if (!resourceExists)
			CopyFileInAssets(originalPath, assetsPath);
	}

	UID uid;

	if (!this->ExistsResourceWithAssetsPath(assetsPath, uid))
	{
		std::shared_ptr<Resource> importedRes = CreateNewResource(fileName, assetsPath, type);
		AddResource(importedRes, originalPath);
		uid = importedRes->GetUID();
	}
	
	return uid;
}

ResourceType ModuleResources::FindTypeByPath(const std::string& path)
{
	std::string fileExtension = GetFileExtension(path);
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

	return ResourceType::Unknown;
}

void ModuleResources::CopyFileInAssets(const std::string& originalPath, const std::string& assetsPath)
{
	//for more protection
	bool exists = App->fileSystem->Exists(assetsPath.c_str());
	if (!exists)
	{
		App->fileSystem->CopyFromOutside(originalPath.c_str(), assetsPath.c_str());
	}
}

bool ModuleResources::ExistsResourceWithAssetsPath(const std::string& assetsPath)
{
	UID willBeIgnored;
	return this->ExistsResourceWithAssetsPath(assetsPath, willBeIgnored);
}

bool ModuleResources::ExistsResourceWithAssetsPath(const std::string& assetsPath, UID& resourceUID)
{
	std::map<UID, std::shared_ptr<Resource> >::iterator it;
	for (it = resources.begin(); it != resources.end(); it++)
	{
		if (it->second->GetAssetsPath() == assetsPath)
		{
			resourceUID = it->second->GetUID();
			return true;
		}
	}
	return false;
}

const std::string ModuleResources::GetPath(const std::string& path)
{
	std::string fileName = "";
	bool separatorFound = false;
	for (int i = path.size() - 1; 0 <= i && !separatorFound; --i)
	{
		char currentChar = path[i];
		separatorFound = currentChar == '\\' || currentChar == '/';
		if (separatorFound)
		{
			fileName = path.substr(0, i + 1);
		}
	}
	return fileName;
}

const std::string ModuleResources::GetFileName(const std::string& path)
{
	std::string fileName = "";
	bool separatorNotFound = true;
	for (int i = path.size() - 1; 0 <= i && separatorNotFound; --i)
	{
		char currentChar = path[i];
		separatorNotFound = currentChar != '\\' && currentChar != '/';
		if (separatorNotFound)
		{
			fileName.insert(0, 1, currentChar);
		}
	}
	std::string fileExtension = GetFileExtension(fileName);
	int posOfExtensionInPath = fileName.find(fileExtension);
	if (posOfExtensionInPath > 0) //has file extension
	{
		fileName.erase(posOfExtensionInPath, fileExtension.size());
	}
	return fileName;
}

const std::string ModuleResources::GetFileExtension(const std::string& path)
{
	std::string fileExtension = "";
	bool dotNotFound = true;
	for (int i = path.size() - 1; dotNotFound && 0 <= i; --i)
	{
		char currentChar = path[i];
		fileExtension.insert(fileExtension.begin(), currentChar);
		dotNotFound = currentChar != '.';
	}
	return fileExtension;
}

const std::string ModuleResources::GetFolderOfType(ResourceType type)
{
	return GetNameOfType(type) + "/";
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
	return ResourceType::Unknown;
}

const std::string ModuleResources::CreateAssetsPath(const std::string& fileName, ResourceType type)
{
	std::string assetsPath = assetsFolder;
	assetsPath += GetFolderOfType(type);
	assetsPath += fileName;
	return assetsPath;
}

const std::string ModuleResources::CreateLibraryPath(const std::string& fileName, ResourceType type)
{
	std::string libraryPath = libraryFolder;
	libraryPath += GetFolderOfType(type);
	libraryPath += fileName;
	return libraryPath;
}

std::shared_ptr<Resource> ModuleResources::CreateNewResource(const std::string& fileName,
															 const std::string& assetsPath,
															 ResourceType type)
{
	UID uid = UniqueID::GenerateUID();
	const std::string libraryPath = CreateLibraryPath(fileName, type);
	return CreateResourceOfType(uid, fileName, assetsPath, libraryPath, type);
}

std::shared_ptr<Resource> ModuleResources::CreateResourceOfType(UID uid,
																const std::string& fileName,
																const std::string& assetsPath,
																const std::string& libraryPath,
																ResourceType type)
{
	switch (type)
	{
	case ResourceType::Model:
		return std::make_shared<ResourceModel>(uid, fileName, assetsPath, libraryPath);
	case ResourceType::Texture:
		return std::make_shared<ResourceTexture>(uid, fileName, assetsPath, libraryPath);
	case ResourceType::Mesh:
		return std::make_shared<ResourceMesh>(uid, fileName, assetsPath, libraryPath);
	case ResourceType::Scene:
	case ResourceType::Material:
		return std::make_shared<ResourceMaterial>(uid, fileName, assetsPath, libraryPath);
		break;
	case ResourceType::SkyBox:
	default:
		return nullptr;
	}
}

void ModuleResources::CreateMetaFileOfResource(const std::shared_ptr<Resource>& resource)
{
	rapidjson::Document doc;
	Json Json(doc, doc);

	Json["UID"] = resource->GetUID();
	Json["Type"] = GetNameOfType(resource->GetType()).c_str();
	resource->SaveOptions(Json);
	rapidjson::StringBuffer buffer;
	Json.toBuffer(buffer);

	std::string path = resource->GetLibraryPath() + META_EXTENSION;

	App->fileSystem->Save(path.c_str(), buffer.GetString(), buffer.GetSize());

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
		break;
	default:
		break;
	}
}
