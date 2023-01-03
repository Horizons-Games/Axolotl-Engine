#pragma once

#include <string>

#include "FileSystem/UniqueID.h"

enum class ResourceType
{
	Unknown,
	Texture,
	Mesh,
	Scene,
	Bone,
	Animation
};

class Resource
{
public:
	virtual ~Resource() = default;

	virtual ResourceType GetType() const = 0;
	UID GetUID() const;
	const std::string& GetAssetsPath() const;
	const std::string& GetLibraryPath() const;

protected:
	Resource(UID resourceUID, const std::string& assetsPath, const std::string& libraryPath);

private:
	UID uid;
	const std::string assetsPath;
	const std::string libraryPath;
};

inline Resource::Resource(UID resourceUID, const std::string& assetsPath, const std::string& libraryPath):
	uid(resourceUID),
	assetsPath(assetsPath),
	libraryPath(libraryPath)
{
}

inline UID Resource::GetUID() const
{
	return uid;
}

inline const std::string& Resource::GetAssetsPath() const
{
	return assetsPath;
}

inline const std::string& Resource::GetLibraryPath() const
{
	return libraryPath;
}
