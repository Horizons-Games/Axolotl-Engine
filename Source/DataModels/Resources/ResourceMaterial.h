#pragma once
#pragma warning(disable : 26495)

#include "Math/float3.h"
#include "Resource.h"

class ResourceTexture;

struct LoadOptionsMaterial
{
};

class ResourceMaterial : virtual public Resource
{
public:
	ResourceMaterial(UID resourceUID,
					 const std::string& fileName,
					 const std::string& assetsPath,
					 const std::string& libraryPath);
	virtual ~ResourceMaterial() override;

	ResourceType GetType() const override;

	void SaveImporterOptions(Json& meta) override{};
	void LoadImporterOptions(Json& meta) override{};

	void SaveLoadOptions(Json& meta) override;
	void LoadLoadOptions(Json& meta) override;

	const std::shared_ptr<ResourceTexture>& GetDiffuse() const;
	const std::shared_ptr<ResourceTexture>& GetNormal() const;
	const std::shared_ptr<ResourceTexture>& GetOcclusion() const;
	const std::shared_ptr<ResourceTexture>& GetMetallic() const;
	const std::shared_ptr<ResourceTexture>& GetSpecular() const;
	const float4& GetDiffuseColor() const;
	const float3& GetSpecularColor() const;
	const float& GetNormalStrength() const;
	const float& GetSmoothness() const;
	const float& GetMetalness() const;
	const bool& GetTransparent() const;
	const unsigned int& GetShaderType() const;

	bool HasDiffuse();
	bool HasNormal();
	bool HasOcclusion();
	bool HasSpecular();
	bool HasMetallic();

	LoadOptionsMaterial& GetLoadOptions();

	void SetDiffuse(const std::shared_ptr<ResourceTexture>& diffuse);
	void SetNormal(const std::shared_ptr<ResourceTexture>& normal);
	void SetOcclusion(const std::shared_ptr<ResourceTexture>& occlusion);
	void SetMetallic(const std::shared_ptr<ResourceTexture>& metallic);
	void SetSpecular(const std::shared_ptr<ResourceTexture>& specular);
	void SetDiffuseColor(const float4& diffuseColor);
	void SetSpecularColor(const float3& specularColor);
	void SetNormalStrength(const float normalStrength);
	void SetSmoothness(const float smoothness);
	void SetMetalness(const float metalness);
	void SetTransparent(const bool isTransparent);
	void SetShaderType(const unsigned int shaderType);

protected:
	void InternalLoad() override{};
	void InternalUnload() override{};

private:
	std::shared_ptr<ResourceTexture> diffuse;
	std::shared_ptr<ResourceTexture> normal;
	std::shared_ptr<ResourceTexture> occlusion;
	std::shared_ptr<ResourceTexture> specular;
	std::shared_ptr<ResourceTexture> metallic;

	float4 diffuseColor;
	float4 oldDiffuseColor;
	float3 specularColor;
	float normalStrength;
	float smoothness;
	float metalness;
	bool isTransparent;
	unsigned int shaderType;

	LoadOptionsMaterial loadOptions;
};

inline ResourceType ResourceMaterial::GetType() const
{
	return ResourceType::Material;
}

inline const std::shared_ptr<ResourceTexture>& ResourceMaterial::GetDiffuse() const
{
	return diffuse;
}

inline const std::shared_ptr<ResourceTexture>& ResourceMaterial::GetNormal() const
{
	return normal;
}

inline const std::shared_ptr<ResourceTexture>& ResourceMaterial::GetOcclusion() const
{
	return occlusion;
}

inline const std::shared_ptr<ResourceTexture>& ResourceMaterial::GetMetallic() const
{
	return metallic;
}

inline const std::shared_ptr<ResourceTexture>& ResourceMaterial::GetSpecular() const
{
	return specular;
}

inline const float4& ResourceMaterial::GetDiffuseColor() const
{
	return diffuseColor;
}

inline const float3& ResourceMaterial::GetSpecularColor() const
{
	return specularColor;
}

inline const float& ResourceMaterial::GetNormalStrength() const
{
	return normalStrength;
}

inline const float& ResourceMaterial::GetSmoothness() const
{
	return smoothness;
}

inline const float& ResourceMaterial::GetMetalness() const
{
	return metalness;
}

inline const bool& ResourceMaterial::GetTransparent() const
{
	return isTransparent;
}

inline const unsigned int& ResourceMaterial::GetShaderType() const
{
	return shaderType;
}

inline LoadOptionsMaterial& ResourceMaterial::GetLoadOptions()
{
	return loadOptions;
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

inline bool ResourceMaterial::HasMetallic()
{
	return metallic != nullptr;
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

inline void ResourceMaterial::SetMetallic(const std::shared_ptr<ResourceTexture>& metallic)
{
	this->metallic = metallic;
}

inline void ResourceMaterial::SetSpecular(const std::shared_ptr<ResourceTexture>& specular)
{
	this->specular = specular;
}

inline void ResourceMaterial::SetDiffuseColor(const float4& diffuseColor)
{
	this->diffuseColor = diffuseColor;
}

inline void ResourceMaterial::SetSpecularColor(const float3& specularColor)
{
	this->specularColor = specularColor;
}

inline void ResourceMaterial::SetNormalStrength(const float normalStrength)
{
	this->normalStrength = normalStrength;
}

inline void ResourceMaterial::SetSmoothness(const float smoothness)
{
	this->smoothness = smoothness;
}

inline void ResourceMaterial::SetMetalness(const float metalness)
{
	this->metalness = metalness;
}

inline void ResourceMaterial::SetTransparent(const bool isTransparent)
{
	this->isTransparent = isTransparent;
}

inline void ResourceMaterial::SetShaderType(const unsigned int shaderType)
{
	this->shaderType = shaderType;
}
