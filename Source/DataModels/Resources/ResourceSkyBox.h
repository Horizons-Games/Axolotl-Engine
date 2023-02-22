#pragma once

#include "Resource.h"
#include <memory>

struct OptionsSkyBox
{
};

class ResourceSkyBox : virtual public Resource
{
public:
	ResourceSkyBox(UID resourceUID, const std::string& fileName, const std::string& assetsPath, const std::string& libraryPath);
	virtual ~ResourceSkyBox() override;

	ResourceType GetType() const override;

	void SaveOptions(Json& meta) override {};
	void LoadOptions(Json& meta) override {};

	void LoadVBO();
	void CreateVAO();

	unsigned int GetGlTexture() const;
	const std::vector<UID>& GetTexturesUIDs() const;
	std::shared_ptr<OptionsSkyBox>& GetOptions();

	void SetTexturesUIDs(const std::vector<UID>& texturesUIDs);

	unsigned int GetVBO() const;
	unsigned int GetVAO() const;

protected:
	void InternalLoad() override;
	void InternalUnload() override;

private:
	unsigned int glTexture = 0;
	std::vector<UID> texturesUIDs;
	std::shared_ptr<OptionsSkyBox> options;

	unsigned vbo;
	unsigned vao;
};

inline ResourceSkyBox::ResourceSkyBox(UID resourceUID,
	const std::string& fileName,
	const std::string& assetsPath,
	const std::string& libraryPath) :
	Resource(resourceUID, fileName, assetsPath, libraryPath),
	texturesUIDs(6),
	options(std::make_shared<OptionsSkyBox>())
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

inline const std::vector<UID>& ResourceSkyBox::GetTexturesUIDs() const
{
	return texturesUIDs;
}

inline std::shared_ptr<OptionsSkyBox>& ResourceSkyBox::GetOptions()
{
	return options;
}

inline void ResourceSkyBox::SetTexturesUIDs(const std::vector<UID>& texturesUIDs)
{
	this->texturesUIDs = texturesUIDs;
}

inline unsigned int ResourceSkyBox::GetVBO() const
{
	return vbo;
}

inline unsigned int ResourceSkyBox::GetVAO() const
{
	return vao;
}