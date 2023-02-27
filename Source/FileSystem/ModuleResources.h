#pragma once
#pragma warning (disable: 4172)

#include "Module.h"

#include <map>
#include <thread>

#include "FileSystem/UniqueID.h"
#include "DataModels/Resources/Resource.h"

#include "Application.h"
#include "ModuleFileSystem.h"

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

	//Create Bin and .meta from path
	void ImportResource(const std::string& originalPath);
	void ImportThread(const std::string& originalPath);

	//request resource and Import if is necessary
	const std::shared_ptr<Resource> RequestResource(const std::string assetPath);
	template<class R>
	const std::shared_ptr<R> RequestResource(const std::string assetPath);

	//Search resource
	const std::shared_ptr<Resource> SearchResource(UID uid);
	template<class R>
	const std::shared_ptr<R> SearchResource(UID uid);

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
	std::shared_ptr<Resource> LoadResourceStored(const char* filePath, const char* fileNameToStore);
	std::shared_ptr<Resource> ImportResourceFromLibrary(const std::string& libraryPath);

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

	ResourceType FindTypeByFolder(const std::string& path);

	//utility methods
	ResourceType FindTypeByExtension(const std::string& path);
	const std::string GetNameOfType(ResourceType type);
	ResourceType GetTypeOfName(const std::string& typeName);
	const std::string GetFolderOfType(ResourceType type);
	const std::string CreateAssetsPath(const std::string& fileName, ResourceType type);
	const std::string CreateLibraryPath(const std::string& fileName, ResourceType type);

	static const std::string assetsFolder;
	static const std::string libraryFolder;
	std::map<UID, std::weak_ptr<Resource> > resources;

	std::unique_ptr<ModelImporter> modelImporter;
	std::unique_ptr<TextureImporter> textureImporter;
	std::unique_ptr<MeshImporter> meshImporter;
	std::unique_ptr<MaterialImporter> materialImporter;
	std::unique_ptr<SkyBoxImporter> skyboxImporter;

	UID skybox;
	
	std::thread monitorThread;
	bool monitorResources = false;

	friend class WindowResources;
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

inline const std::shared_ptr<Resource> ModuleResources::RequestResource(const std::string assetPath)
{
	return RequestResource<Resource>(assetPath);
}

inline const std::shared_ptr<Resource> ModuleResources::SearchResource(UID uid)
{
	return SearchResource<Resource>(uid);
}

template<class R>
inline const std::shared_ptr<R> ModuleResources::SearchResource(UID uid)
{
	auto it = resources.find(uid);
	if (it != resources.end())
	{
		std::shared_ptr<Resource> shared = (it->second).lock();
		return std::dynamic_pointer_cast<R>(shared);
	}
	
	return std::dynamic_pointer_cast<R>(LoadResourceStored(LIB_FOLDER, std::to_string(uid).c_str()));
}