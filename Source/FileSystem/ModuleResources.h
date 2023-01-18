#pragma once
#pragma warning (disable: 4172)

#include "Module.h"

#include <string>
#include <memory>
#include <map>
#include <thread>

#include "FileSystem/UniqueID.h"
#include "DataModels/Resources/Resource.h"

class ModelImporter;
class TextureImporter;
class MeshImporter;
class MaterialImporter;
class SkyBoxImporter;

class ResourceMaterial;

enum class ResourceType;

class ModuleResources : public Module
{
public:
	ModuleResources();
	~ModuleResources() override;

	//inherited methods from Module
	bool Start() override;
	bool CleanUp() override;

	//import resource from path
	UID ImportResource(const std::string& originalPath);
	UID ImportThread(const std::string& originalPath);

	//request resoruce
	const std::weak_ptr<Resource> RequestResource(UID uid);
	template<class R>
	const std::weak_ptr<R> RequestResource(UID uid);
	const UID GetSkyBoxResource();

private:
	//resource creation and deletition
	std::shared_ptr<Resource> CreateNewResource(const std::string& fileName,
												const std::string& assetsPath,
												ResourceType type);
	std::shared_ptr<Resource> CreateResourceOfType(UID uid,
												   const std::string& fileName,
												   const std::string& assetsPath,
												   const std::string& libraryPath,
												   ResourceType type);
	void AddResource(std::shared_ptr<Resource>& resource, const std::string& originalPath);
	void DeleteResource(UID uidToDelete);

	//create resources from binaries
	void LoadResourceStored(const char* filePath);
	void ImportResourceFromLibrary(const std::string& libraryPath);

	//importing: creation of binary and meta
	void CreateMetaFileOfResource(const std::shared_ptr<Resource>& resource);
	void ImportResourceFromSystem(const std::string& originalPath,
								  std::shared_ptr<Resource>& resource,
								  ResourceType type);

	//folder and file management
	void CreateAssetAndLibFolders();
	void MonitorResources();
	void ReImportMaterialAsset(const std::shared_ptr<ResourceMaterial>& materialResource);
	bool ExistsResourceWithAssetsPath(const std::string& assetsPath, UID& resourceUID);

	//utility methods
	ResourceType FindTypeByPath(const std::string& path);
	const std::string GetNameOfType(ResourceType type);
	ResourceType GetTypeOfName(const std::string& typeName);
	const std::string GetFolderOfType(ResourceType type);
	const std::string CreateAssetsPath(const std::string& fileName, ResourceType type);
	const std::string CreateLibraryPath(const std::string& fileName, ResourceType type);

	static const std::string assetsFolder;
	static const std::string libraryFolder;
	std::map<UID, std::shared_ptr<Resource> > resources;

	std::unique_ptr<ModelImporter> modelImporter;
	std::unique_ptr<TextureImporter> textureImporter;
	std::unique_ptr<MeshImporter> meshImporter;
	std::unique_ptr<MaterialImporter> materialImporter;
	std::unique_ptr<SkyBoxImporter> skyboxImporter;

	UID skybox;
	
	std::thread monitorThread;
	bool monitorResources = false;
};

inline bool ModuleResources::CleanUp()
{
#if !defined(GAME)
	monitorResources = false;
	monitorThread.join();
#endif
	resources.clear();
	return true;
}

inline const std::weak_ptr<Resource> ModuleResources::RequestResource(UID uid)
{
	return RequestResource<Resource>(uid);
}

template<class R>
inline const std::weak_ptr<R> ModuleResources::RequestResource(UID uid)
{
	auto it = resources.find(uid);
	if (it != resources.end())
	{
		return std::dynamic_pointer_cast<R>(it->second);
	}
	//empty weak_ptr
	return std::weak_ptr<R>();
}