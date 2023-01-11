#pragma once

#include "Module.h"

#include <string>
#include <memory>
#include <map>

#include "FileSystem/UniqueID.h"

class Resource;
class ModelImporter;
class TextureImporter;
class MeshImporter;

enum class ResourceType;

class ModuleResources : public Module
{
public:
	ModuleResources() = default;
	~ModuleResources() override = default;

	bool Start() override;
	bool CleanUp() override;

	UID ImportThread(const std::string& originalPath);
	UID ImportResource(const std::string& originalPath);

	const std::shared_ptr<Resource>& RequestResource(UID uid);

private:
	void MonitorResources();
	void LoadResourceStored(const char* filePath);
	ResourceType FindTypeByPath(const std::string& path);
	void CopyFileInAssets(const std::string& originalPath, const std::string& assetsPath);
	//this might not belong here
	const std::string GetFileName(const std::string& path);
	const std::string GetFileExtension(const std::string& path);
	const std::string GetFolderOfType(ResourceType type);
	const std::string CreateAssetsPath(const std::string& fileName, ResourceType type);
	const std::string CreateLibraryPath(const std::string& fileName, ResourceType type);
	std::shared_ptr<Resource> CreateNewResource(const std::string& fileName, const std::string& assetsPath, ResourceType type);
	void CreateMetaFileOfResource(const std::shared_ptr<Resource>& resource);
	void ImportResourceFromSystem(std::shared_ptr<Resource>& resource, ResourceType type);

	static const std::string assetsFolder;
	static const std::string libraryFolder;
	std::map<UID, std::shared_ptr<Resource> > resources;

	std::shared_ptr<ModelImporter> modelImporter;
	std::shared_ptr<TextureImporter> textureImporter;
	std::shared_ptr<MeshImporter> meshImporter;

	bool monitorResources;
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
