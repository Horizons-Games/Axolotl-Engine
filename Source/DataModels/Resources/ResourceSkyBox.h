#pragma once

#include "Resource.h"
#include <memory>

struct OptionsSkyBox
{
};

class ResourceSkyBox : public Resource
{
public:
	ResourceSkyBox(UID resourceUID, const std::string& fileName, const std::string& assetsPath, const std::string& libraryPath);
	~ResourceSkyBox() override;

	ResourceType GetType() const override;

	void SaveOptions(Json& meta) override {};
	void LoadOptions(Json& meta) override {};

	void LoadVBO();
	void CreateVAO();

	unsigned int GetGlTexture() const;
	const std::vector<std::shared_ptr<Resource>>& GetTextures() const;
	std::unique_ptr<OptionsSkyBox>& GetOptions();

	void SetTextures(const std::vector<std::shared_ptr<Resource>>& textures);

	unsigned int GetVBO() const;
	unsigned int GetVAO() const;

protected:
	void InternalLoad() override;
	void InternalUnload() override;

private:
	unsigned int glTexture = 0;
	std::vector<std::shared_ptr<Resource>> textures;
	std::unique_ptr<OptionsSkyBox> options;

	unsigned vbo;
	unsigned vao;
};

inline ResourceSkyBox::ResourceSkyBox(UID resourceUID,
	const std::string& fileName,
	const std::string& assetsPath,
	const std::string& libraryPath) :
	Resource(resourceUID, fileName, assetsPath, libraryPath),
	options(std::make_unique<OptionsSkyBox>())
{
}

inline ResourceSkyBox::~ResourceSkyBox()
{
	Unload();
}

inline ResourceType ResourceSkyBox::GetType() const
{
	return ResourceType::SkyBox;
}

inline unsigned int ResourceSkyBox::GetGlTexture() const
{
	return glTexture;
}

inline const std::vector<std::shared_ptr<Resource>>& ResourceSkyBox::GetTextures() const
{
	return textures;
}

inline std::unique_ptr<OptionsSkyBox>& ResourceSkyBox::GetOptions()
{
	return options;
}

inline void ResourceSkyBox::SetTextures(const std::vector<std::shared_ptr<Resource>>& textures)
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