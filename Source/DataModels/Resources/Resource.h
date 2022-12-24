#pragma once

#include <string>

//while waiting for the UUID class, ints will be used as placeholder

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
	int GetUUID() const;
	const std::string& GetAssetsPath() const;
	const std::string& GetLibraryPath() const;

protected:
	Resource(int resourceUUID, const std::string& assetsPath, const std::string& libraryPath);

private:
	int uuid;
	const std::string assetsPath;
	const std::string libraryPath;
};

inline Resource::Resource(int resourceUUID, const std::string& assetsPath, const std::string& libraryPath):
	uuid(resourceUUID),
	assetsPath(assetsPath),
	libraryPath(libraryPath)
{
}

inline int Resource::GetUUID() const
{
	return uuid;
}

inline const std::string& Resource::GetAssetsPath() const
{
	return assetsPath;
}

inline const std::string& Resource::GetLibraryPath() const
{
	return libraryPath;
}
