#pragma once

#include "Resource.h"

class ResourceTexture;

class ResourceCubemap : virtual public Resource
{
public:
	ResourceCubemap(UID resourceUID,
					const std::string& fileName,
					const std::string& assetsPath,
					const std::string& libraryPath);
	virtual ~ResourceCubemap() override;

	ResourceType GetType() const override;

	void SaveImporterOptions(Json& meta) override{};
	void LoadImporterOptions(Json& meta) override{};

	void SaveLoadOptions(Json& meta) override{};
	void LoadLoadOptions(Json& meta) override{};

	std::shared_ptr<ResourceTexture> GetHDRTexture() const;

	void SetHDRTexture(const std::shared_ptr<ResourceTexture>& textures);

protected:
	void InternalLoad() override;
	void InternalUnload() override;

	bool ChildChanged() const override;

private:
	std::shared_ptr<ResourceTexture> hdrTexture;
};

inline ResourceType ResourceCubemap::GetType() const
{
	return ResourceType::Cubemap;
}

inline std::shared_ptr<ResourceTexture> ResourceCubemap::GetHDRTexture() const
{
	return hdrTexture;
}

inline void ResourceCubemap::SetHDRTexture(const std::shared_ptr<ResourceTexture>& hdrTexture)
{
	this->hdrTexture = hdrTexture;
}
