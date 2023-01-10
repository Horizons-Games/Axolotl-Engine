#include "ModuleResources.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "Json.h"

#include "FileSystem/Importers/Importer.h"
#include "FileSystem/Importers/ModelImporter.h"
#include "FileSystem/Importers/MeshImporter.h"
#include "FileSystem/Importers/TextureImporter.h"

#include "Resources/Resource.h"
#include "Resources/ResourceMesh.h"
#include "Resources/ResourceTexture.h"
#include <thread>
#include <future>

const std::string ModuleResources::assetsFolder = "Assets/";
const std::string ModuleResources::libraryFolder = "Lib/";

bool ModuleResources::Start()
{
	modelImporter = std::make_shared<ModelImporter>();
	textureImporter = std::make_shared<TextureImporter>();
	meshImporter = std::make_shared<MeshImporter>();

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
	std::vector<ResourceType> allResourceTypes = {ResourceType::Animation,
												  ResourceType::Bone,
												  ResourceType::Material,
												  ResourceType::Mesh,
												  ResourceType::Model,
												  ResourceType::Scene,
												  ResourceType::Texture};
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
	importThread.join();
	return f.get();
}

UID ModuleResources::ImportResource(const std::string& originalPath)
{
	ResourceType type = FindTypeByPath(originalPath);
	std::string fileName = GetFileName(originalPath);
	std::string extension = GetFileExtension(originalPath);
	std::string assetsPath = originalPath;

	if (type != ResourceType::Mesh) 
	{
		assetsPath = CreateAssetsPath(fileName + extension, type);

		bool resourceExists = App->fileSystem->Exists(assetsPath.c_str());
		if (!resourceExists)
			CopyFileInAssets(originalPath, assetsPath);
	}

		

	std::shared_ptr<Resource> importedRes = CreateNewResource(fileName, assetsPath, type);
	CreateMetaFileOfResource(importedRes);
	ImportResourceFromSystem(importedRes, type);

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

const std::string ModuleResources::GetFileName(const std::string& path)
{
	std::string fileName = "";
	bool separatorNotFound = true;
	bool notExtension = false;
	for (int i = path.size() - 1; 0 <= i && separatorNotFound; --i)
	{
		char currentChar = path[i];
		separatorNotFound = currentChar != '/';
		if (separatorNotFound && notExtension)
		{
			fileName.insert(0, 1, currentChar);
		}
		if(!notExtension) notExtension = currentChar == '.';
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
	case ResourceType::Bone:
		return "Bones/";
	case ResourceType::Animation:
		return "Animations/";
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
		break;
	case ResourceType::Bone:
		break;
	case ResourceType::Animation:
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
	rapidjson::StringBuffer buffer;
	Json.toBuffer(buffer);

	std::string path = resource->GetLibraryPath() + META_EXTENSION;

	App->fileSystem->Save(path.c_str(), buffer.GetString(), buffer.GetSize());

}

void ModuleResources::ImportResourceFromSystem(std::shared_ptr<Resource>& resource, ResourceType type)
{
	switch (type)
	{
	case ResourceType::Model:
		modelImporter->Import(resource->GetAssetsPath().c_str(), std::dynamic_pointer_cast<ResourceModel>(resource));
		break;
	case ResourceType::Texture:
		textureImporter->Import(resource->GetAssetsPath().c_str(), std::dynamic_pointer_cast<ResourceTexture>(resource));
		break;
	case ResourceType::Mesh:
		meshImporter->Import(resource->GetAssetsPath().c_str(), std::dynamic_pointer_cast<ResourceMesh>(resource));
		break;
	case ResourceType::Scene:
		break;
	case ResourceType::Material:
		break;
	case ResourceType::Bone:
		break;
	case ResourceType::Animation:
		break;
	default:
		break;
	}
}
