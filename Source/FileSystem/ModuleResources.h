#pragma once
#include "Module.h"

#include "DataModels/Resources/Resource.h"

#include "Application.h"
#include "Json.h"
#include "ModuleFileSystem.h"

#include "Defines/ExtensionDefines.h"
#include "Defines/FileSystemDefines.h"

class ModelImporter;
class TextureImporter;
class VideoImporter;
class MeshImporter;
class MaterialImporter;
class SkyBoxImporter;
class CubemapImporter;
class AnimationImporter;
class StateMachineImporter;
class ParticleSystemImporter;

class ResourceMaterial;
class EditorResourceInterface;

enum class ResourceType;

class ModuleResources : public Module
{
public:
	ModuleResources();
	~ModuleResources() override;

	// inherited methods from Module
	bool Init() override;
	bool CleanUp() override;

	void CreateDefaultResource(ResourceType type, const std::string& fileName);

	// Create Bin and .meta from path
	std::shared_ptr<Resource> ImportResource(const std::string& originalPath);
	std::future<std::shared_ptr<Resource>> ImportThread(const std::string& originalPath);

	// request resource and Import if is necessary
	template<class R = Resource>
	const std::shared_ptr<R> RequestResource(const std::string assetPath);

	// Search resource
	template<class R = Resource>
	const std::shared_ptr<R> SearchResource(UID uid);

	void ReimportResource(UID resourceUID);

	void FillResourceBin(std::shared_ptr<Resource> sharedResource);
	void CleanResourceBin();

private:
	// resource creation and deletition
	std::shared_ptr<Resource>
		CreateNewResource(const std::string& fileName, const std::string& assetsPath, ResourceType type);
	std::shared_ptr<Resource> CreateResourceOfType(UID uid,
												   const std::string& fileName,
												   const std::string& assetsPath,
												   const std::string& libraryPath,
												   ResourceType type);
	void AddResource(std::shared_ptr<Resource>& resource, const std::string& originalPath);
	// I wont guard this with #ifdef beacuse this needs to go on a different Module that will only exist in ENGINE
	void DeleteResource(const std::shared_ptr<EditorResourceInterface>& resToDelete);

	// create resources from binaries
	std::shared_ptr<Resource> LoadResourceStored(const char* filePath, const char* fileNameToStore);
	void ImportResourceFromLibrary(std::shared_ptr<Resource>& resource);

	// importing: creation of binary and meta
	void CreateMetaFileOfResource(std::shared_ptr<Resource>& resource);
	void ImportResourceFromSystem(const std::string& originalPath,
								  std::shared_ptr<Resource>& resource,
								  ResourceType type);

	// folder and file management
	void CreateAssetAndLibFolders();
	void MonitorResources();
	bool ExistsResourceWithAssetsPath(const std::string& assetsPath, UID& resourceUID);

	ResourceType FindTypeByFolder(const std::string& path);

	// utility methods
	ResourceType FindTypeByExtension(const std::string& path);
	const std::string GetNameOfType(ResourceType type);
	ResourceType GetTypeOfName(const std::string& typeName);
	const std::string GetFolderOfType(ResourceType type);
	const std::string CreateAssetsPath(const std::string& fileName, ResourceType type);
	const std::string CreateLibraryPath(UID resourceUID, ResourceType type);

	static const std::string assetsFolder;
	static const std::string libraryFolder;
	std::map<UID, std::weak_ptr<Resource>> resources;

	std::unique_ptr<ModelImporter> modelImporter;
	std::unique_ptr<VideoImporter> videoImporter;
	std::unique_ptr<TextureImporter> textureImporter;
	std::unique_ptr<MeshImporter> meshImporter;
	std::unique_ptr<MaterialImporter> materialImporter;
	std::unique_ptr<SkyBoxImporter> skyboxImporter;
	std::unique_ptr<CubemapImporter> cubemapImporter;
	std::unique_ptr<AnimationImporter> animationImporter;
	std::unique_ptr<StateMachineImporter> stateMachineImporter;
	std::unique_ptr<ParticleSystemImporter> particleSystemImporter;

	std::thread monitorThread;
	bool monitorResources;

	std::vector<std::shared_ptr<Resource>> resourcesBin;

	friend class WindowResources;
};

template<class R>
const std::shared_ptr<R> ModuleResources::RequestResource(const std::string path)
{
	ResourceType type = FindTypeByExtension(path);
	if (type == ResourceType::Unknown)
	{
		LOG_WARNING("Extension not supported");
	}
	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();
	std::string fileName = fileSystem->GetFileName(path);
	std::string extension = fileSystem->GetFileExtension(path);
	std::string assetPath = CreateAssetsPath(fileName + extension, type);

	std::string metaPath = assetPath + META_EXTENSION;
	if (fileSystem->Exists(metaPath.c_str()))
	{
		char* metaBuffer = {};
		fileSystem->Load(metaPath.c_str(), metaBuffer);

		rapidjson::Document doc;
		Json meta(doc, doc);

		meta.fromBuffer(metaBuffer);
		delete metaBuffer;

		UID uid = (UID) meta["UID"];

		// If that resource is already on the map because another component uses it, we return it
		auto it = resources.find(uid);
		if (it != resources.end() && !(it->second).expired())
		{
			return std::dynamic_pointer_cast<R>(it->second.lock());
		}

		// If that resource has binaries and they are new, we load them
		ResourceType type = GetTypeOfName(std::string(meta["Type"]));

		std::string libraryPath = CreateLibraryPath(uid, type);

		long long assetTime = fileSystem->GetModificationDate(assetPath.c_str());
		long long libTime;
		if (type == ResourceType::Video)
		{
			libTime = fileSystem->GetModificationDate((libraryPath + extension).c_str());
		}
		else
			libTime = fileSystem->GetModificationDate((libraryPath + GENERAL_BINARY_EXTENSION).c_str());
		if (assetTime <= libTime)
		{
			std::string fileName = fileSystem->GetFileName(libraryPath + GENERAL_BINARY_EXTENSION);
			UID uid = std::stoull(fileName.c_str(), NULL, 0);
			ResourceType type = FindTypeByFolder(libraryPath + GENERAL_BINARY_EXTENSION);
			std::shared_ptr<Resource> resource =
				CreateResourceOfType(uid, fileSystem->GetFileName(assetPath), assetPath, libraryPath, type);
			resource->LoadImporterOptions(meta);
			resource->LoadLoadOptions(meta);
			ImportResourceFromLibrary(resource);

			if (resource)
			{
				resources.insert({ resource->GetUID(), resource });
				return std::move(std::dynamic_pointer_cast<R>(resource));
			}
		}
	}

	// If that resource does not have any of the two options, we will import it again
	ImportResource(path);
	metaPath = assetPath + META_EXTENSION;
	if (fileSystem->Exists(metaPath.c_str()))
	{
		char* metaBuffer = {};
		fileSystem->Load(metaPath.c_str(), metaBuffer);

		rapidjson::Document doc;
		Json meta(doc, doc);

		meta.fromBuffer(metaBuffer);
		delete metaBuffer;

		UID uid = (UID) meta["UID"];
		ResourceType type = GetTypeOfName(std::string(meta["Type"]));

		std::string libraryPath = CreateLibraryPath(uid, type);

		long long assetTime = fileSystem->GetModificationDate(assetPath.c_str());
		long long libTime = fileSystem->GetModificationDate((libraryPath + GENERAL_BINARY_EXTENSION).c_str());
		if (assetTime <= libTime)
		{
			std::string fileName = fileSystem->GetFileName(libraryPath + GENERAL_BINARY_EXTENSION);
			UID uid = std::stoull(fileName.c_str(), NULL, 0);
			ResourceType type = FindTypeByFolder(libraryPath + GENERAL_BINARY_EXTENSION);
			std::shared_ptr<Resource> resource =
				CreateResourceOfType(uid, fileSystem->GetFileName(assetPath), assetPath, libraryPath, type);
			resource->LoadImporterOptions(meta);
			resource->LoadLoadOptions(meta);
			ImportResourceFromLibrary(resource);
			if (resource)
			{
				resources.insert({ resource->GetUID(), resource });
				return std::move(std::dynamic_pointer_cast<R>(resource));
			}
		}
	}
	return nullptr;
}

inline void ModuleResources::CleanResourceBin()
{
#ifndef ENGINE
	resourcesBin.clear();
#endif //! ENGINE
}

template<class R>
const std::shared_ptr<R> ModuleResources::SearchResource(UID uid)
{
	auto it = resources.find(uid);
	if (it != resources.end() && !(it->second).expired())
	{
		std::shared_ptr<Resource> shared = (it->second).lock();
		return std::move(std::dynamic_pointer_cast<R>(shared));
	}

	std::shared_ptr<Resource> shared = LoadResourceStored(LIB_FOLDER, std::to_string(uid).c_str());
	if (shared)
	{
		resources.insert({ shared->GetUID(), shared });
		return std::move(std::dynamic_pointer_cast<R>(shared));
	}
	return nullptr;
}
