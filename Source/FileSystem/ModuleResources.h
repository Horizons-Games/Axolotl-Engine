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
	std::shared_ptr<Resource> ImportResource(const std::string& originalPath);
	std::shared_ptr<Resource> ImportThread(const std::string& originalPath);

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
	void LoadResourceStored(const char* filePath);
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

	//utility methods
	ResourceType FindTypeByPath(const std::string& path);
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

template<class R>
inline const std::shared_ptr<R> ModuleResources::RequestResource(const std::string assetPath)
{
	//Si ese recurso ya esta en el map porque otro componente lo usa lo devolvemos
	std::string metaPath = assetPath + META_EXTENSION;
	if (App->fileSystem->Exists(metaPath.c_str())) {
		char* metaBuffer = {};
		App->fileSystem->Load(metaPath.c_str(), metaBuffer);

		rapidjson::Document doc;
		Json Json(doc, doc);

		Json.fromBuffer(metaBuffer);

		UID uid = (UID)Json["UID"];

		auto it = resources.find(uid);
		if (it != resources.end())
		{
			return std::dynamic_pointer_cast<R>(resources.find(resourceUID)->second.lock());
		}
	}

	//Si ese recurso tiene binarios y son nuevos los cargamos
	std::shared_ptr<Resource> resource = ImportResourceFromLibrary(assetPath);
	if (resource)
	{
		long long assetTime = App->fileSystem->GetModificationDate(assetPath.c_str());
		long long libTime = App->fileSystem->GetModificationDate(resource->GetLibraryPath().c_str());
		if(assetTime < libTime) 
		{
			resources.insert({ resource->GetUID(), resource });
			return std::dynamic_pointer_cast<R>(resource);
		}
	}

	//Si ese recurso no tiene ninguna de las dos opciones lo volvemos a importar
	resource = ImportResource(assetPath);
	resources.insert({ resource->GetUID(), resource });
	return std::dynamic_pointer_cast<R>(resource);
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
	


	return std::shared_ptr<R>();
}