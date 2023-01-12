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
		std::vector<std::string> toImport;
		std::map<UID, std::shared_ptr<Resource> >::iterator it;
		for (it = resources.begin(); it != resources.end(); it++)
		{
			if (!App->fileSystem->Exists(it->second->GetAssetsPath().c_str()))
			{
				toRemove.push_back(it->first);
			}
			else if (App->fileSystem->Exists(it->second->GetLibraryPath().c_str()))
			{
				toImport.push_back(it->second->GetAssetsPath());
			}
			else if (App->fileSystem->Exists((it->second->GetLibraryPath() + META_EXTENSION).c_str()))
			{
				toImport.push_back(it->second->GetAssetsPath());
			}
			else
			{
				long long assetTime = App->fileSystem->GetModificationDate(it->second->GetAssetsPath().c_str());
				long long libTime = App->fileSystem->GetModificationDate((it->second->GetLibraryPath() + META_EXTENSION).c_str());
				if (assetTime > libTime)
				{
					toImport.push_back(it->second->GetAssetsPath());
				}
			}
		}
		//Remove resources
		for (size_t i = 0; i < toRemove.size(); i++)
		{
			std::string libPath = resources[toRemove[i]]->GetAssetsPath();
			std::string metaPath = resources[toRemove[i]]->GetAssetsPath() + META_EXTENSION;
			App->fileSystem->Delete(metaPath.c_str());
			App->fileSystem->Delete(libPath.c_str());
			resources.erase(toRemove[i]);

		}
		//Import resources
		for (size_t i = 0; i < toImport.size(); i++)
		{
			ImportResource(toImport[i]);
		}
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
				ImportResource(file);
			}
		}
	}
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
	//(actually there is a library that looks really clean but might be overkill: https://github.com/Neargye/magic_enum)
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
	//LoadResourceStored(libraryFolder.c_str());
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
	std::string fileName = GetFileName(originalPath);
	std::string extension = GetFileExtension(originalPath);
	std::string assetsPath = originalPath;

	if (type != ResourceType::Mesh && type != ResourceType::Material) 
	{
		assetsPath = CreateAssetsPath(fileName + extension, type);

		bool resourceExists = App->fileSystem->Exists(assetsPath.c_str());
		if (!resourceExists)
			CopyFileInAssets(originalPath, assetsPath);
	}


	std::shared_ptr<Resource> importedRes = CreateNewResource(fileName, assetsPath, type);
	CreateMetaFileOfResource(importedRes);

	ImportResourceFromSystem(originalPath, importedRes, type);

	UID uid = importedRes->GetUID();
	resources.insert({ uid, importedRes });
	return uid;
}

ResourceType ModuleResources::FindTypeByPath(const std::string& path)
{
	const std::string fileExtension = GetFileExtension(path);
	
	if (fileExtension == MODEL_EXTENSION) 
	{
		return ResourceType::Model;
	}
	else if (fileExtension == JPG_TEXTURE_EXTENSION || fileExtension == PNG_TEXTURE_EXTENSION || 
			fileExtension == TIF_TEXTURE_EXTENSION || fileExtension == DDS_TEXTURE_EXTENSION || 
			fileExtension == TGA_TEXTURE_EXTENSION) 
	{
		return ResourceType::Texture;
	}
	else if (fileExtension == SCENE_EXTENSION) 
	{
		return ResourceType::Scene;
	}
	else if (fileExtension == MATERIAL_EXTENSION)
	{
		return ResourceType::Material;
	}
	else if (fileExtension == MESH_EXTENSION)
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
		App->fileSystem->Copy(originalPath.c_str(), assetsPath.c_str());
	}
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
	bool extension = true;
	for (int i = path.size() - 1; 0 <= i && separatorNotFound; --i)
	{
		char currentChar = path[i];
		separatorNotFound = currentChar != '\\' && currentChar != '/';
		if (separatorNotFound && !extension)
		{
			fileName.insert(0, 1, currentChar);
		}
		if(extension) extension = currentChar != '.';
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
	switch (type)
	{
	case ResourceType::Model:
		return "Models/";
	case ResourceType::Texture:
		return "Textures/";
	case ResourceType::Mesh:
		return "Meshes/";
	case ResourceType::Scene:
		return "Scenes/";
	case ResourceType::Material:
		return "Materials/";
	case ResourceType::SkyBox:
		return "SkyBox/";
	default:
		return "";
	}
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

std::shared_ptr<Resource> ModuleResources::CreateNewResource(const std::string& fileName, const std::string& assetsPath, ResourceType type)
{
	UID uid = UniqueID::GenerateUID();
	const std::string libraryPath = CreateLibraryPath(fileName, type);
	std::shared_ptr<Resource> resource = nullptr;
	switch (type)
	{
	case ResourceType::Model:
		resource = std::make_shared<ResourceModel>(uid, fileName, assetsPath, libraryPath);
		break;
	case ResourceType::Texture:
		resource = std::make_shared<ResourceTexture>(uid, fileName, assetsPath, libraryPath);
		break;
	case ResourceType::Mesh:
		resource = std::make_shared<ResourceMesh>(uid, fileName, assetsPath, libraryPath);
		break;
	case ResourceType::Scene:
		break;
	case ResourceType::Material:
		resource = std::make_shared<ResourceMaterial>(uid, fileName, assetsPath, libraryPath);
		break;
	case ResourceType::SkyBox:
		break;
	default:
		break;
	}
	return resource;
}

void ModuleResources::CreateMetaFileOfResource(const std::shared_ptr<Resource>& resource)
{
	rapidjson::Document doc;
	Json Json(doc, doc);

	Json["UID"] = resource->GetUID();
	resource->SaveOptions(Json);
	rapidjson::StringBuffer buffer;
	Json.toBuffer(buffer);

	std::string path = resource->GetLibraryPath() + META_EXTENSION;

	App->fileSystem->Save(path.c_str(), buffer.GetString(), buffer.GetSize());

}

void ModuleResources::ImportResourceFromSystem(const std::string& originalPath, std::shared_ptr<Resource>& resource, ResourceType type)
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
