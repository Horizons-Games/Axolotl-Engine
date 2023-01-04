#include "ModuleResources.h"

#include "Application.h"
#include "ModuleFileSystem.h"

#include "Resources/Resource.h"
#include "Resources/ResourceTexture.h"

UID ModuleResources::ImportResource(const std::string& originalPath)
{
	ResourceType type = FindTypeByPath(originalPath);
	std::string fileName = GetFileName(originalPath);
	std::string assetsPath = CreateAssetsPath(fileName, type);

	bool resourceExists = App->fileSystem->Exists(assetsPath.c_str());
	if (!resourceExists)
		CopyFileInAssets(originalPath);

	std::shared_ptr<Resource> importedRes = CreateNewResource(assetsPath, type);
	CreateMetaFileOfResource(importedRes);
	ImportResourceFromSystem(importedRes, type);
	UID uid = importedRes->GetUID();
	resources.insert({ uid, importedRes });
	return uid;
}

ResourceType ModuleResources::FindTypeByPath(const std::string& path)
{
	const std::string fileExtension = GetFileExtension(path);
	//add ifs for each extension, return corresponding type
	return ResourceType::Unknown;
}

const std::string ModuleResources::CopyFileInAssets(const std::string& originalPath)
{
	// TODO: Insertar una instrucción "return" aquí
	return "";
}

const std::string ModuleResources::GetFileName(const std::string& path)
{
	std::string fileName = "";
	bool separatorNotFound = true;
	for (int i = path.size() - 1; 0 <= i && separatorNotFound; --i)
	{
		char currentChar = path[i];
		separatorNotFound = currentChar != '\\';
		if (separatorNotFound)
		{
			fileName.insert(0, 1, currentChar);
		}
	}
	return fileName;
}

const std::string& ModuleResources::GetFileExtension(const std::string& path)
{
	std::string fileExtension;
	bool dotNotFound = true;
	for (int i = path.size() - 1; dotNotFound && 0 <= i; --i)
	{
		char currentChar = path[i];
		fileExtension.insert(fileExtension.begin(), currentChar);
		dotNotFound = currentChar != '.';
	}
	return fileExtension;
}

const std::string ModuleResources::CreateAssetsPath(const std::string& fileName, ResourceType type)
{
	// TODO: create the path
	switch (type)
	{
	case ResourceType::Texture:
		break;
	case ResourceType::Mesh:
		break;
	case ResourceType::Scene:
		break;
	case ResourceType::Bone:
		break;
	case ResourceType::Animation:
		break;
	default:
		break;
	}
	return "";
}

const std::string ModuleResources::CreateLibraryPath(UID resourceUID, ResourceType type)
{
	// TODO: create the path
	switch (type)
	{
	case ResourceType::Texture:
		break;
	case ResourceType::Mesh:
		break;
	case ResourceType::Scene:
		break;
	case ResourceType::Bone:
		break;
	case ResourceType::Animation:
		break;
	default:
		break;
	}
	return "";
}

std::shared_ptr<Resource> ModuleResources::CreateNewResource(const std::string& assetsPath, ResourceType type)
{
	UID uid = UniqueID::GenerateUID();
	const std::string libraryPath = CreateLibraryPath(uid, type);
	std::shared_ptr<Resource> resource = nullptr;
	switch (type)
	{
	case ResourceType::Texture:
		resource = std::make_shared<ResourceTexture>(uid, assetsPath, libraryPath);
		break;
	case ResourceType::Mesh:
		break;
	case ResourceType::Scene:
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
}

void ModuleResources::ImportResourceFromSystem(const std::shared_ptr<Resource>& resource, ResourceType type)
{
	switch (type)
	{
	case ResourceType::Texture:
		break;
	case ResourceType::Mesh:
		break;
	case ResourceType::Scene:
		break;
	case ResourceType::Bone:
		break;
	case ResourceType::Animation:
		break;
	default:
		break;
	}
}
