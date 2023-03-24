#pragma once

#include "Resource.h"
#include <memory>

class ResourceTexture;

struct OptionsSkyBox
{
};

class ResourceSkyBox : virtual public Resource
{
public:
	ResourceSkyBox(UID resourceUID, 
		const std::string& fileName, 
		const std::string& assetsPath, 
		const std::string& libraryPath);
	virtual ~ResourceSkyBox() override;

	ResourceType GetType() const override;

	void SaveImporterOptions(Json& meta) override {};
	void LoadImporterOptions(Json& meta) override {};

	void LoadVBO();
	void CreateVAO();

	unsigned int GetGlTexture() const;
	const std::vector<std::shared_ptr<ResourceTexture>>& GetTextures() const;
	OptionsSkyBox& GetOptions();

	void SetTextures(const std::vector<std::shared_ptr<ResourceTexture>>& textures);

	unsigned int GetVBO() const;
	unsigned int GetVAO() const;

protected:
	void InternalLoad() override;
	void InternalUnload() override;

	bool ChildChanged() const override;

private:
	unsigned int glTexture = 0;
	std::vector<std::shared_ptr<ResourceTexture>> textures;
	OptionsSkyBox options;

	unsigned vbo;
	unsigned vao;
};

inline ResourceType ResourceSkyBox::GetType() const
{
	return ResourceType::SkyBox;
}

inline unsigned int ResourceSkyBox::GetGlTexture() const
{
	return glTexture;
}

inline const std::vector<std::shared_ptr<ResourceTexture>>& ResourceSkyBox::GetTextures() const
{
	return textures;
}

inline OptionsSkyBox& ResourceSkyBox::GetOptions()
{
	return options;
}

inline void ResourceSkyBox::SetTextures(const std::vector<std::shared_ptr<ResourceTexture>>& textures)
{
	this->textures = textures;
}

inline unsigned int ResourceSkyBox::GetVBO() const
{
	return vbo;
}

inline unsigned int ResourceSkyBox::GetVAO() const
{
	return vao;
}