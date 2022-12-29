#include "ModuleResources.h"

#include "Resources/Resource.h"
#include "Resources/ResourceTexture.h"

int ModuleResources::ImportResource(const std::string& assetsPath)
{
	ResourceType type = FindTypeByPath(assetsPath);
	std::shared_ptr<Resource> importedRes = CreateNewResource(assetsPath, type);
	int uuid = importedRes->GetUUID();
	resources.insert({ uuid, importedRes });
	return uuid;
}

ResourceType ModuleResources::FindTypeByPath(const std::string& assetsPath)
{
	const std::string fileExtension = GetFileExtension(assetsPath);
	//add ifs for each extension, return corresponding type
	return ResourceType::Unknown;
}

const std::string& ModuleResources::GetFileExtension(const std::string& assetsPath)
{
	std::string fileExtension;
	bool dotNotFound = true;
	for (int i = assetsPath.size() - 1; dotNotFound && 0 <= i; --i) {
		char currentChar = assetsPath[i];
		fileExtension.insert(fileExtension.begin(), currentChar);
		dotNotFound = currentChar != '.';
	}
	return fileExtension;
}

const std::string& ModuleResources::CreateLibraryPath(const std::string& assetsPath)
{
	// TODO: create the path
	return "";
}

std::shared_ptr<Resource> ModuleResources::CreateNewResource(const std::string& assetsPath, ResourceType type)
{
	int uuid = 0;
	const std::string libraryPath = CreateLibraryPath(assetsPath);
	std::shared_ptr<Resource> resource = nullptr;
	switch (type)
	{
	case ResourceType::Texture:
		resource = std::make_shared<ResourceTexture>(uuid, assetsPath, libraryPath);
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
