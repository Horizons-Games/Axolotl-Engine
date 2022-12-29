#pragma once

#include "Module.h"

#include <string>
#include <memory>
#include <map>

class Resource;

enum class ResourceType;

class ModuleResources : public Module
{
public:
	ModuleResources() = default;
	~ModuleResources() override = default;

	bool CleanUp() override;

	int ImportResource(const std::string& assetsPath);

	const std::shared_ptr<Resource>& RequestResource(int uuid);

private:
	ResourceType FindTypeByPath(const std::string& assetsPath);
	//this might not belong here
	const std::string& GetFileExtension(const std::string& assetsPath);
	const std::string& CreateLibraryPath(const std::string& assetsPath);
	std::shared_ptr<Resource> CreateNewResource(const std::string& assetsPath, ResourceType type);

	std::map<int, std::shared_ptr<Resource> > resources;
};

inline bool ModuleResources::CleanUp()
{
	resources.clear();
}

inline const std::shared_ptr<Resource>& ModuleResources::RequestResource(int uuid)
{
	auto it = resources.find(uuid);
	if (it != resources.end())
	{
		return it->second;
	}
	//should do something different
	return nullptr;
}
