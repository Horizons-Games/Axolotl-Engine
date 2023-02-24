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
	ResourceMaterial(UID resourceUID, 
					const std::string& fileName, 
					const std::string& assetsPath, 
					const std::string& libraryPath);
	~ResourceMaterial() override;

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

inline ResourceType ResourceMaterial::GetType() const
{
	return ResourceType::Material;
}

inline UID& ResourceMaterial::GetDiffuseUID()
{
	return diffuseUID;
}

inline UID& ResourceMaterial::GetNormalUID()
{
	return normalUID;
}

inline UID& ResourceMaterial::GetOcclusionrUID()
{
	return occlusionUID;
}

inline UID& ResourceMaterial::GetSpecularUID()
{
	return specularUID;
}

inline float3& ResourceMaterial::GetDiffuseColor()
{
	return diffuseColor;
}

inline float3& ResourceMaterial::GetSpecularColor()
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
	hasDiffuse = true;
}

inline void ResourceMaterial::SetNormalUID(UID& normalUID)
{
	this->normalUID = normalUID;
	hasNormal = true;
}

inline void ResourceMaterial::SetOcclusionUID(UID& occlusionUID)
{
	this->occlusionUID = occlusionUID;
	occlusionUID = true;
}

inline void ResourceMaterial::SetSpecularUID(UID& specularUID)
{
	this->specularUID = specularUID;
	hasSpecular = true;
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
