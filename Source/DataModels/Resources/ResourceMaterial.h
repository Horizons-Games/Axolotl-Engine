#pragma once
#pragma warning (disable: 26495)

#include "Resource.h"
#include "Math/float3.h"

class ResourceTexture;

struct OptionsMaterial
{
};

class ResourceMaterial : public Resource
{
public:
	ResourceMaterial(UID resourceUID, 
					const std::string& fileName, 
					const std::string& assetsPath, 
					const std::string& libraryPath);
	~ResourceMaterial() override;

	ResourceType GetType() const override;

	void SaveOptions(Json& meta) override {};
	void LoadOptions(Json& meta) override {};

	std::shared_ptr<ResourceTexture>& GetDiffuse();
	std::shared_ptr<ResourceTexture>& GetNormal();
	std::shared_ptr<ResourceTexture>& GetOcclusion();
	std::shared_ptr<ResourceTexture>& GetSpecular();
	const float3& GetDiffuseColor();
	const float3& GetSpecularColor();
	float& GetShininess();
	float& GetNormalStrength();
	bool HasDiffuse();
	bool HasNormal();
	bool HasOcclusion();
	bool HasSpecular();
	bool HasShininessAlpha();

	std::shared_ptr<OptionsMaterial>& GetOptions();

	//Sets
	void SetDiffuse(const std::shared_ptr<ResourceTexture>& diffuse);
	void SetNormal(const std::shared_ptr<ResourceTexture>& normal);
	void SetOcclusion(const std::shared_ptr<ResourceTexture>& occlusion);
	void SetSpecular(const std::shared_ptr<ResourceTexture>& specular);
	void SetDiffuseColor(float3& diffuseColor);
	void SetSpecularColor(float3& specularColor);
	void SetShininess(float shininess);
	void SetNormalStrength(float normalStrength);
	void SetShininessAlpha(bool shininessAlpha);

protected:
	void InternalLoad() override {};
	void InternalUnload() override {};

private:

	std::shared_ptr<ResourceTexture> diffuse;
	std::shared_ptr<ResourceTexture> normal;
	std::shared_ptr<ResourceTexture> occlusion;
	std::shared_ptr<ResourceTexture> specular;
	float3 diffuseColor;
	float3 specularColor;
	float shininess;
	float normalStrength;

	bool shininessAlpha;

	std::shared_ptr<OptionsMaterial> options;
};

inline ResourceType ResourceMaterial::GetType() const
{
	return ResourceType::Material;
}

inline std::shared_ptr<ResourceTexture>& ResourceMaterial::GetDiffuse()
{
	return this->diffuse;
}

inline std::shared_ptr<ResourceTexture>& ResourceMaterial::GetNormal()
{
	return this->normal;
}

inline std::shared_ptr<ResourceTexture>& ResourceMaterial::GetOcclusion()
{
	return this->occlusion;
}

inline std::shared_ptr<ResourceTexture>& ResourceMaterial::GetSpecular()
{
	return this->specular;
}

inline const float3& ResourceMaterial::GetDiffuseColor()
{
	return diffuseColor;
}

inline const float3& ResourceMaterial::GetSpecularColor()
{
	return specularColor;
}

inline float& ResourceMaterial::GetShininess()
{
	return shininess;
}

inline float& ResourceMaterial::GetNormalStrength()
{
	return normalStrength;
}

inline std::shared_ptr<OptionsMaterial>& ResourceMaterial::GetOptions()
{
	return options;
}

inline bool ResourceMaterial::HasDiffuse()
{
	return diffuse != nullptr;
}

inline bool ResourceMaterial::HasNormal()
{
	return normal != nullptr;
}

inline bool ResourceMaterial::HasOcclusion()
{
	return occlusion != nullptr;
}

inline bool ResourceMaterial::HasSpecular()
{
	return specular != nullptr;
}

inline bool ResourceMaterial::HasShininessAlpha()
{
	return shininessAlpha;  
}

inline void ResourceMaterial::SetDiffuse(const std::shared_ptr<ResourceTexture>& diffuse)
{
	this->diffuse = diffuse;
}

inline void ResourceMaterial::SetNormal(const std::shared_ptr<ResourceTexture>& normal)
{
	this->normal = normal;
}

inline void ResourceMaterial::SetOcclusion(const std::shared_ptr<ResourceTexture>& occlusion)
{
	this->occlusion = occlusion;
}

inline void ResourceMaterial::SetSpecular(const std::shared_ptr<ResourceTexture>& specular)
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

inline void ResourceMaterial::SetShininess(float shininess)
{
	this->shininess = shininess;
}

inline void ResourceMaterial::SetNormalStrength(float normalStrength)
{
	this->normalStrength = normalStrength;
}

inline void ResourceMaterial::SetShininessAlpha(bool shininessAlpha)
{
	this->shininessAlpha = shininessAlpha;
}
