#pragma once
#pragma warning (disable: 26495)

#include "Resource.h"
#include "Math/float3.h"

struct OptionsMaterial
{
};

class ResourceMaterial : public Resource
{
public:
	ResourceMaterial(UID resourceUID, const std::string& fileName, const std::string& assetsPath, const std::string& libraryPath);
	~ResourceMaterial() override;

	ResourceType GetType() const override;

	void SaveOptions(Json& meta) override {};
	void LoadOptions(Json& meta) override {};

	std::shared_ptr<Resource>& GetDiffuse();
	std::shared_ptr<Resource>& GetNormal();
	std::shared_ptr<Resource>& GetOcclusion();
	std::shared_ptr<Resource>& GetSpecular();
	float3& GetDiffuseColor();
	float3& GetSpecularColor();
	float& GetShininess();
	float& GetNormalStrength();
	bool haveDiffuse();
	bool haveNormal();
	bool haveOcclusion();
	bool haveSpecular();
	bool HaveShininessAlpha();


	std::shared_ptr<OptionsMaterial>& GetOptions();

	//Sets
	void SetDiffuse(const std::shared_ptr<Resource>& diffuse);
	void SetNormal(const std::shared_ptr<Resource>& normal);
	void SetOcclusion(const std::shared_ptr<Resource>& occlusion);
	void SetSpecular(const std::shared_ptr<Resource>& specular);
	void SetDiffuseColor(float3& diffuseColor);
	void SetSpecularColor(float3& specularColor);
	void SetShininess(float& shininess);
	void SetNormalStrength(float& normalStrength);

protected:
	void InternalLoad() override {};
	void InternalUnload() override {};

private:

	std::shared_ptr<Resource> diffuse;
	std::shared_ptr<Resource> normal;
	std::shared_ptr<Resource> occlusion;
	std::shared_ptr<Resource> specular;
	float3 diffuseColor;
	float3 specularColor;
	float shininess;
	float normalStrength;

	bool shininessAlpha;

	std::shared_ptr<OptionsMaterial> options;
};

inline ResourceMaterial::ResourceMaterial(UID resourceUID,
	const std::string& fileName,
	const std::string& assetsPath,
	const std::string& libraryPath) :
	Resource(resourceUID, fileName, assetsPath, libraryPath)
{
	options = std::make_shared<OptionsMaterial>();
	diffuseColor = float3(1.0f, 1.0f, 0.0f);
	specularColor = float3(0.5f, 0.3f, 0.5f);
	shininess = 512.f;
	normalStrength = 1.0f;
	shininessAlpha = false;
}

inline ResourceMaterial::~ResourceMaterial()
{
	this->Unload();
}

inline ResourceType ResourceMaterial::GetType() const
{
	return ResourceType::Material;
}

inline std::shared_ptr<Resource>& ResourceMaterial::GetDiffuse()
{
	return this->diffuse;
}

inline std::shared_ptr<Resource>& ResourceMaterial::GetNormal()
{
	return this->normal;
}

inline std::shared_ptr<Resource>& ResourceMaterial::GetOcclusion()
{
	return this->occlusion;
}

inline std::shared_ptr<Resource>& ResourceMaterial::GetSpecular()
{
	return this->specular;
}

inline float3& ResourceMaterial::GetDiffuseColor()
{
	return this->diffuseColor;
}

inline float3& ResourceMaterial::GetSpecularColor()
{
	return this->specularColor;
}

inline float& ResourceMaterial::GetShininess()
{
	return this->shininess;
}

inline float& ResourceMaterial::GetNormalStrength()
{
	return this->normalStrength;
}

inline std::shared_ptr<OptionsMaterial>& ResourceMaterial::GetOptions()
{
	return this->options;
}

inline bool ResourceMaterial::haveDiffuse()
{
	return diffuse != nullptr;
}

inline bool ResourceMaterial::haveNormal()
{
	return normal != nullptr;
}

inline bool ResourceMaterial::haveOcclusion()
{
	return occlusion != nullptr;
}

inline bool ResourceMaterial::haveSpecular()
{
	return specular != nullptr;
}

inline bool ResourceMaterial::HaveShininessAlpha()
{
	return shininessAlpha;  
}

inline void ResourceMaterial::SetDiffuse(const std::shared_ptr<Resource>& diffuse)
{
	this->diffuse = diffuse;
}

inline void ResourceMaterial::SetNormal(const std::shared_ptr<Resource>& normal)
{
	this->normal = normal;
}

inline void ResourceMaterial::SetOcclusion(const std::shared_ptr<Resource>& occlusion)
{
	this->occlusion = occlusion;
}

inline void ResourceMaterial::SetSpecular(const std::shared_ptr<Resource>& specular)
{
	this->specular = specular;
}

inline void ResourceMaterial::SetDiffuseColor(float3& diffuseColor)
{
	this->diffuseColor = diffuseColor;
}

inline void ResourceMaterial::SetSpecularColor(float3& specularColor)
{
	this->specularColor = specularColor;
}

inline void ResourceMaterial::SetShininess(float& shininess)
{
	this->shininess = shininess;
}

inline void ResourceMaterial::SetNormalStrength(float& normalStrength)
{
	this->normalStrength = normalStrength;
}
