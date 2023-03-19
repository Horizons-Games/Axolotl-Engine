#pragma once

#include "FileSystem/UniqueID.h"

class Json;

enum class ResourceType
{
	Unknown,
	Model,
	Texture,
	Mesh,
	Scene,
	Material,
	SkyBox,
	Window,
};

class Resource
{
public:
	virtual ~Resource() = default;

	virtual ResourceType GetType() const = 0;
	UID GetUID() const;
	//name without extension
	const std::string& GetFileName() const;
	//path with extension
	const std::string& GetAssetsPath() const;
	//path without extension (extension will always be GENERAL_BINARY_EXTENSION)
	const std::string& GetLibraryPath() const;

	void Load();
	void Unload();
	virtual void SaveOptions(Json& meta) = 0;
	virtual void LoadOptions(Json& meta) = 0;

	bool IsLoaded() const;

	bool IsChanged() const;
	void SetChanged(bool changed);

protected:
	Resource(UID resourceUID, 
		const std::string& fileName, 
		const std::string& assetsPath, 
		const std::string& libraryPath);

	virtual void InternalLoad() = 0;
	virtual void InternalUnload() = 0;

	bool changed = false;

private:
	UID uid;
	bool loaded = false;
	const std::string fileName;
	const std::string assetsPath;
	const std::string libraryPath;
};

inline bool Resource::IsLoaded() const
{
	return loaded;
}

inline bool Resource::IsChanged() const
{
	return changed;
}

inline void Resource::SetChanged(bool changed)
{
	this->changed = changed;
}

inline Resource::Resource(	UID resourceUID, 
							const std::string& fileName, 
							const std::string& assetsPath, 
							const std::string& libraryPath):
	uid(resourceUID),
	fileName(fileName),
	assetsPath(assetsPath),
	libraryPath(libraryPath)
{
}

inline UID Resource::GetUID() const
{
	return uid;
}

inline const std::string& Resource::GetFileName() const
{
	return fileName;
}

inline const std::string& Resource::GetAssetsPath() const
{
	return assetsPath;
}

inline const std::string& Resource::GetLibraryPath() const
{
	return libraryPath;
}

inline void Resource::Load()
{
	if (!loaded)
	{
		InternalLoad();
		loaded = true;
	}
}

inline void Resource::Unload()
{
	if (loaded)
	{
		InternalUnload();
		loaded = false;
	}
}
