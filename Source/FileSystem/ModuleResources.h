#pragma once

#include "Module.h"

#include <string>
#include <memory>
#include <map>

#include "FileSystem/UniqueID.h"

class Resource;

enum class ResourceType;

class ModuleResources : public Module
{
public:
	ModuleResources() = default;
	~ModuleResources() override = default;

	bool CleanUp() override;

	UID ImportResource(const std::string& originalPath);

	const std::shared_ptr<Resource>& RequestResource(UID uid);

private:
	ResourceType FindTypeByPath(const std::string& path);
	const std::string CopyFileInAssets(const std::string& originalPath);
	//this might not belong here
	const std::string GetFileName(const std::string& path);
	const std::string GetFileExtension(const std::string& path);
	const std::string CreateAssetsPath(const std::string& fileName, ResourceType type);
	const std::string CreateLibraryPath(UID resourceUID, ResourceType type);
	std::shared_ptr<Resource> CreateNewResource(const std::string& assetsPath, ResourceType type);
	void CreateMetaFileOfResource(const std::shared_ptr<Resource>& resource);
	void ImportResourceFromSystem(const std::shared_ptr<Resource>& resource, ResourceType type);

	std::map<UID, std::shared_ptr<Resource> > resources;
};

inline bool ModuleResources::CleanUp()
{
	resources.clear();
	return true;
}

inline const std::shared_ptr<Resource>& ModuleResources::RequestResource(UID uid)
{
	auto it = resources.find(uid);
	if (it != resources.end())
	{
		return it->second;
	}
	//should do something different
	return nullptr;
}
