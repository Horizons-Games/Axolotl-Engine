#pragma once

#include "Module.h"

#include <string>
#include <memory>
#include <map>
#include <thread>

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

	const std::weak_ptr<Resource>& RequestResource(UID uid);
	const std::string GetPath(const std::string& path);

private:
	void MonitorResources();
	void LoadResourceStored(const char* filePath);
	void ImportResourceFromLibrary(const std::string& libraryPath);
	void AddResource(std::shared_ptr<Resource>& resource, const std::string& originalPath);
	ResourceType FindTypeByPath(const std::string& path);
	void CopyFileInAssets(const std::string& originalPath, const std::string& assetsPath);
	bool ExistsResourceWithAssetsPath(const std::string& assetsPath);
	bool ExistsResourceWithAssetsPath(const std::string& assetsPath, UID& resourceUID);
	//this might not belong here
	const std::string GetFileName(const std::string& path);
	const std::string GetFileExtension(const std::string& path);
	const std::string GetFolderOfType(ResourceType type);
	const std::string GetNameOfType(ResourceType type);
	ResourceType GetTypeOfName(const std::string& typeName);
	const std::string CreateAssetsPath(const std::string& fileName, ResourceType type);
	const std::string CreateLibraryPath(const std::string& fileName, ResourceType type);
	std::shared_ptr<Resource> CreateNewResource(const std::string& fileName,
												const std::string& assetsPath,
												ResourceType type);
	std::shared_ptr<Resource> CreateResourceOfType(UID uid,
												   const std::string& fileName,
												   const std::string& assetsPath,
												   const std::string& libraryPath,
												   ResourceType type);
	void CreateMetaFileOfResource(const std::shared_ptr<Resource>& resource);
	void ImportResourceFromSystem(const std::string& originalPath,
								  std::shared_ptr<Resource>& resource,
								  ResourceType type);

	static const std::string assetsFolder;
	static const std::string libraryFolder;
	std::map<UID, std::shared_ptr<Resource> > resources;

	std::shared_ptr<ModelImporter> modelImporter;
	std::shared_ptr<TextureImporter> textureImporter;
	std::shared_ptr<MeshImporter> meshImporter;
	
	//std::thread monitorThread;
	bool monitorResources;
};

inline bool ModuleResources::CleanUp()
{
	monitorResources = false;
	//monitorThread.join();
	resources.clear();
	return true;
}

inline const std::weak_ptr<Resource>& ModuleResources::RequestResource(UID uid)
{
	auto it = resources.find(uid);
	if (it != resources.end())
	{
		return it->second;
	}
	//empty weak_ptr
	return std::weak_ptr<Resource>();
}
