#pragma once
#include "Resource.h"

class ResourceTexture;

struct LoadOptionsMaterial
{
	float4 diffuseColor;
	float3 specularColor;
	float normalStrength;
	float smoothness;
	float metalness;
	bool isTransparent;
	float2 tiling;
	float2 offset;
	float2 percentage;

	unsigned int shaderType; //This is a special option because it's both load and import option

	LoadOptionsMaterial() : 
		diffuseColor(float4(1.0f, 1.0f, 0.0f, 1.0f)),
		specularColor(float3(0.5f, 0.3f, 0.5f)),
		normalStrength(1.0f),
		smoothness(0.5f),
		metalness(0.0f),
		isTransparent(false),
		tiling(float2(1.0f)),
		offset(float2(0.0f)),
		percentage(float2(100.0f)),
		shaderType(0)
	{
	}
};

class ResourceMaterial : virtual public Resource
{
public:
	ResourceMaterial(UID resourceUID,
					 const std::string& fileName,
					 const std::string& assetsPath,
					 const std::string& libraryPath);
	virtual ~ResourceMaterial() override;
	void CopyValues(const ResourceMaterial& rhs);

	ResourceType GetType() const override;

	void SaveImporterOptions(Json& meta) override{};
	void LoadImporterOptions(Json& meta) override{};

	void SaveLoadOptions(Json& meta) override;
	void LoadLoadOptions(Json& meta) override;

	// Resources with child resources like this, their children are part of the load options
	// because their path is a load option provided by the asset and editable in the engine
	void SavePaths(Json& meta, const std::vector<std::string>& pathTextures = std::vector<std::string>());
	void LoadPaths(Json& meta);

	std::shared_ptr<ResourceTexture> GetDiffuse() const;
	std::shared_ptr<ResourceTexture> GetNormal() const;
	std::shared_ptr<ResourceTexture> GetOcclusion() const;
	std::shared_ptr<ResourceTexture> GetMetallic() const;
	std::shared_ptr<ResourceTexture> GetSpecular() const;
	std::shared_ptr<ResourceTexture> GetEmission() const;

	bool HasDiffuse();
	bool HasNormal();
	bool HasOcclusion();
	bool HasSpecular();
	bool HasMetallic();
	bool HasEmissive();

	LoadOptionsMaterial& GetLoadOptions();
	const float4& GetDiffuseColor() const;
	const float3& GetSpecularColor() const;
	const float& GetNormalStrength() const;
	const float& GetSmoothness() const;
	const float& GetMetalness() const;
	const bool& IsTransparent() const;
	const unsigned int& GetShaderType() const;
	const float2& GetTiling() const;
	const float2& GetOffset() const;
	const float2& GetPercentage() const;

	void SetLoadOptions(LoadOptionsMaterial& options);
	void SetDiffuse(const std::shared_ptr<ResourceTexture>& diffuse);
	void SetNormal(const std::shared_ptr<ResourceTexture>& normal);
	void SetOcclusion(const std::shared_ptr<ResourceTexture>& occlusion);
	void SetMetallic(const std::shared_ptr<ResourceTexture>& metallic);
	void SetSpecular(const std::shared_ptr<ResourceTexture>& specular);
	void SetEmission(const std::shared_ptr<ResourceTexture>& emission);
	void SetDiffuseColor(const float4& diffuseColor);
	void SetSpecularColor(const float3& specularColor);
	void SetNormalStrength(const float normalStrength);
	void SetSmoothness(const float smoothness);
	void SetMetalness(const float metalness);
	void SetTransparent(const bool isTransparent);
	void SetShaderType(const unsigned int shaderType);
	void SetTiling(const float2& tiling);
	void SetOffset(const float2& offset);
	void SetPercentage(const float2& percentage);
	
protected:
	void InternalLoad() override{};
	void InternalUnload() override{};

private:
	bool CheckAssetPath(std::string& assetPath);
	
	std::shared_ptr<ResourceTexture> diffuse;
	std::shared_ptr<ResourceTexture> normal;
	std::shared_ptr<ResourceTexture> occlusion;
	std::shared_ptr<ResourceTexture> specular;
	std::shared_ptr<ResourceTexture> metallic;
	std::shared_ptr<ResourceTexture> emission;

	LoadOptionsMaterial loadOptions;
};

inline ResourceType ResourceMaterial::GetType() const
{
	return ResourceType::Material;
}

inline std::shared_ptr<ResourceTexture> ResourceMaterial::GetDiffuse() const
{
	return diffuse;
}

inline std::shared_ptr<ResourceTexture> ResourceMaterial::GetNormal() const
{
	return normal;
}

inline std::shared_ptr<ResourceTexture> ResourceMaterial::GetOcclusion() const
{
	return occlusion;
}

inline std::shared_ptr<ResourceTexture> ResourceMaterial::GetMetallic() const
{
	return metallic;
}

inline std::shared_ptr<ResourceTexture> ResourceMaterial::GetSpecular() const
{
	return specular;
}

inline std::shared_ptr<ResourceTexture> ResourceMaterial::GetEmission() const
{
	return emission;
}

inline const float4& ResourceMaterial::GetDiffuseColor() const
{
	return loadOptions.diffuseColor;
}

inline const float3& ResourceMaterial::GetSpecularColor() const
{
	return loadOptions.specularColor;
}

inline const float& ResourceMaterial::GetNormalStrength() const
{
	return loadOptions.normalStrength;
}

inline const float& ResourceMaterial::GetSmoothness() const
{
	return loadOptions.smoothness;
}

inline const float& ResourceMaterial::GetMetalness() const
{
	return loadOptions.metalness;
}

inline const bool& ResourceMaterial::IsTransparent() const
{
	return loadOptions.isTransparent;
}

inline const unsigned int& ResourceMaterial::GetShaderType() const
{
	return loadOptions.shaderType;
}

inline const float2& ResourceMaterial::GetTiling() const
{
	return loadOptions.tiling;
}

inline const float2& ResourceMaterial::GetOffset() const
{
	return loadOptions.offset;
}

inline const float2& ResourceMaterial::GetPercentage() const
{
	return loadOptions.percentage;
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

inline bool ResourceMaterial::HasEmissive()
{
	return emission != nullptr;
}

inline void ResourceMaterial::SetLoadOptions(LoadOptionsMaterial& options)
{
	loadOptions = options;
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

inline void ResourceMaterial::SetEmission(const std::shared_ptr<ResourceTexture>& emission)
{
	this->emission = emission;
}

inline void ResourceMaterial::SetDiffuseColor(const float4& diffuseColor)
{
	loadOptions.diffuseColor = diffuseColor;
}

inline void ResourceMaterial::SetSpecularColor(const float3& specularColor)
{
	loadOptions.specularColor = specularColor;
}

inline void ResourceMaterial::SetNormalStrength(const float normalStrength)
{
	loadOptions.normalStrength = normalStrength;
}

inline void ResourceMaterial::SetSmoothness(const float smoothness)
{
	loadOptions.smoothness = smoothness;
}

inline void ResourceMaterial::SetMetalness(const float metalness)
{
	loadOptions.metalness = metalness;
}

inline void ResourceMaterial::SetTransparent(const bool isTransparent)
{
	loadOptions.isTransparent = isTransparent;
}

inline void ResourceMaterial::SetShaderType(const unsigned int shaderType)
{
	if (shaderType > 1)
	{
		loadOptions.shaderType = 0;
	}
	else
	{
		loadOptions.shaderType = shaderType;
	}
}

inline void ResourceMaterial::SetTiling(const float2& tiling)
{
	loadOptions.tiling = tiling;
}

inline void ResourceMaterial::SetOffset(const float2& offset)
{
	loadOptions.offset = offset;
}

inline void ResourceMaterial::SetPercentage(const float2& percentage)
{
	loadOptions.percentage = percentage;
}
