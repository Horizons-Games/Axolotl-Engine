#pragma once

#include "Resource.h"
#include "Math/float3.h"

struct OptionsMaterial
{
};

class ResourceMaterial : public Resource
{
public:
	ResourceMaterial(UID resourceUID, const std::string& fileName, const std::string& assetsPath, const std::string& libraryPath);
	~ResourceMaterial() override = default;

	ResourceType GetType() const override;

	void SaveOptions(Json& meta) override {};
	void LoadOptions(Json& meta) override {};

	UID& GetDiffuseUID();
	UID& GetNormalUID();
	UID& GetOcclusionrUID();
	UID& GetSpecularUID();
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
	void SetDiffuseUID(UID& diffuseUID);
	void SetNormalUID(UID& normalUID);
	void SetOcclusionUID(UID& occlusionUID);
	void SetSpecularUID(UID& specularUID);
	void SetDiffuseColor(float3& diffuseColor);
	void SetSpecularColor(float3& specularColor);
	void SetShininess(float& shininess);
	void SetNormalStrength(float& normalStrength);

protected:
	void InternalLoad() override {};
	void InternalUnload() override {};

private:

	UID diffuseUID;
	UID normalUID;
	UID occlusionUID;
	UID specularUID;
	float3 diffuseColor;
	float3 specularColor;
	float shininess;
	float normalStrength;

	//TODO change UID or leave this here meanwhile this is for MaterialImporter and Component Material charge
	bool hasDiffuse;
	bool hasNormal;
	bool hasOcclusion;
	bool hasSpecular;
	bool shininessAlpha;

	std::shared_ptr<OptionsMaterial> options;
};

inline ResourceMaterial::ResourceMaterial(UID resourceUID,
	const std::string& fileName,
	const std::string& assetsPath,
	const std::string& libraryPath) :
	Resource(resourceUID, fileName, assetsPath, libraryPath)
{
	diffuseUID = 0;
	normalUID = 0;
	occlusionUID = 0;
	specularUID = 0;
	options = std::make_shared<OptionsMaterial>();
	diffuseColor = float3(1.0, 1.0, 0.0);
	specularColor = float3(0.5, 0.3, 0.5);
	shininess = 512.f;
	normalStrength = 1.0f;
	hasDiffuse = false;
	hasNormal = false;
	hasOcclusion = false;
	hasSpecular = false;
	shininessAlpha = false;
}

inline ResourceType ResourceMaterial::GetType() const
{
	return ResourceType::Material;
}

inline UID& ResourceMaterial::GetDiffuseUID()
{
	return this->diffuseUID;
}

inline UID& ResourceMaterial::GetNormalUID()
{
	return this->normalUID;
}

inline UID& ResourceMaterial::GetOcclusionrUID()
{
	return this->occlusionUID;
}

inline UID& ResourceMaterial::GetSpecularUID()
{
	return this->specularUID;
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
	return hasDiffuse;
}

inline bool ResourceMaterial::haveNormal()
{
	return hasNormal;
}

inline bool ResourceMaterial::haveOcclusion()
{
	return hasOcclusion;
}

inline bool ResourceMaterial::haveSpecular()
{
	return hasSpecular;
}

inline bool ResourceMaterial::HaveShininessAlpha()
{
	return shininessAlpha;  
}

inline void ResourceMaterial::SetDiffuseUID(UID& diffuseUID)
{
	this->diffuseUID = diffuseUID;
	this->hasDiffuse = true;
}

inline void ResourceMaterial::SetNormalUID(UID& normalUID)
{
	this->normalUID = normalUID;
	this->hasNormal = true;
}

inline void ResourceMaterial::SetOcclusionUID(UID& occlusionUID)
{
	this->occlusionUID = occlusionUID;
	this->occlusionUID = true;
}

inline void ResourceMaterial::SetSpecularUID(UID& specularUID)
{
	this->specularUID = specularUID;
	this->hasSpecular = true;
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
