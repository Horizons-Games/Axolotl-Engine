#include "StdAfx.h"
#include "ResourceTexture.h"
#include "ResourceMaterial.h"
#include "FileSystem/Json.h"
#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"

ResourceMaterial::ResourceMaterial(UID resourceUID,
								   const std::string& fileName,
								   const std::string& assetsPath,
								   const std::string& libraryPath) :
	Resource(resourceUID, fileName, assetsPath, libraryPath)
{
}

ResourceMaterial::~ResourceMaterial()
{
	this->Unload();
}

void ResourceMaterial::CopyValues(const ResourceMaterial& rhs)
{
	this->SetShaderType(rhs.GetShaderType());
	this->SetDiffuseColor(rhs.GetDiffuseColor());
	this->SetSpecularColor(rhs.GetSpecularColor());
	this->SetNormalStrength(rhs.GetNormalStrength());
	this->SetSmoothness(rhs.GetSmoothness());
	this->SetMetalness(rhs.GetMetalness());
	this->SetTransparent(rhs.IsTransparent());
	this->SetTiling(rhs.GetTiling());
	this->SetOffset(rhs.GetOffset());
	this->SetPercentage(rhs.GetPercentage());
	this->SetDiffuse(rhs.GetDiffuse());
	this->SetNormal(rhs.GetNormal());
	this->SetOcclusion(rhs.GetOcclusion());
	this->SetSpecular(rhs.GetSpecular());
	this->SetMetallic(rhs.GetMetallic());
	this->SetEmission(rhs.GetEmission());
}

void ResourceMaterial::SaveLoadOptions(Json& meta)
{
	SavePaths(meta);

	meta["shaderType"] = static_cast<int>(loadOptions.shaderType);
	meta["diffuseColor_x"] = static_cast<float>(loadOptions.diffuseColor.x);
	meta["diffuseColor_y"] = static_cast<float>(loadOptions.diffuseColor.y);
	meta["diffuseColor_z"] = static_cast<float>(loadOptions.diffuseColor.z);
	meta["diffuseColor_w"] = static_cast<float>(loadOptions.diffuseColor.w);
	meta["specularColor_x"] = static_cast<float>(loadOptions.specularColor.x);
	meta["specularColor_y"] = static_cast<float>(loadOptions.specularColor.y);
	meta["specularColor_z"] = static_cast<float>(loadOptions.specularColor.z);
	meta["normalStrength"] = static_cast<float>(loadOptions.normalStrength);
	meta["smoothness"] = static_cast<float>(loadOptions.smoothness);
	meta["metalness"] = static_cast<float>(loadOptions.metalness);
	meta["isTransparent"] = static_cast<bool>(loadOptions.isTransparent);
	meta["tilingx"] = static_cast<float>(loadOptions.tiling.x);
	meta["tilingy"] = static_cast<float>(loadOptions.tiling.y);
	meta["offsetx"] = static_cast<float>(loadOptions.offset.x);
	meta["offsety"] = static_cast<float>(loadOptions.offset.y);
	meta["percentagex"] = static_cast<float>(loadOptions.percentage.x);
	meta["percentagey"] = static_cast<float>(loadOptions.percentage.y);
}

void ResourceMaterial::LoadLoadOptions(Json& meta)
{
	loadOptions.shaderType = static_cast<int>(meta["shaderType"]);

	LoadPaths(meta);

	loadOptions.diffuseColor.x = static_cast<float>(meta["diffuseColor_x"]);
	loadOptions.diffuseColor.y = static_cast<float>(meta["diffuseColor_y"]);
	loadOptions.diffuseColor.z = static_cast<float>(meta["diffuseColor_z"]);
	loadOptions.diffuseColor.w = static_cast<float>(meta["diffuseColor_w"]);
	loadOptions.specularColor.x = static_cast<float>(meta["specularColor_x"]);
	loadOptions.specularColor.y = static_cast<float>(meta["specularColor_y"]);
	loadOptions.specularColor.z = static_cast<float>(meta["specularColor_z"]);
	loadOptions.normalStrength = static_cast<float>(meta["normalStrength"]);
	loadOptions.smoothness = static_cast<float>(meta["smoothness"]);
	loadOptions.metalness = static_cast<float>(meta["metalness"]);
	loadOptions.isTransparent = static_cast<bool>(meta["isTransparent"]);
	loadOptions.tiling.x = static_cast<float>(meta["tilingx"]);
	loadOptions.tiling.y = static_cast<float>(meta["tilingy"]);
	loadOptions.offset.x = static_cast<float>(meta["offsetx"]);
	loadOptions.offset.y = static_cast<float>(meta["offsety"]);
	loadOptions.percentage.x = static_cast<float>(meta["percentagex"]);
	loadOptions.percentage.y = static_cast<float>(meta["percentagey"]);
	if (loadOptions.percentage.x == 0.f && loadOptions.percentage.y == 0.f)
	{
		loadOptions.percentage.y = 100.f;
		loadOptions.percentage.x = 100.f;
	}
}

void ResourceMaterial::SavePaths(Json& meta, const std::vector<std::string>& pathTextures)
{
	meta["DiffuseAssetPath"] =
		pathTextures.empty() ? (diffuse ? diffuse->GetAssetsPath().c_str() : "") : pathTextures[0].c_str();
	meta["NormalAssetPath"] =
		pathTextures.empty() ? (normal ? normal->GetAssetsPath().c_str() : "") : pathTextures[1].c_str();
	meta["OcclusionAssetPath"] =
		pathTextures.empty() ? (occlusion ? occlusion->GetAssetsPath().c_str() : "") : pathTextures[2].c_str();
	int shaderType = pathTextures.empty() ? loadOptions.shaderType : 0;
	switch (shaderType)
	{
		case 0:
			meta["MetalicAssetPath"] =
				pathTextures.empty() ? (metallic ? metallic->GetAssetsPath().c_str() : "") : pathTextures[3].c_str();
			break;
		case 1:
			meta["SpecularAssetPath"] =
				pathTextures.empty() ? (specular ? specular->GetAssetsPath().c_str() : "") : pathTextures[3].c_str();
			break;
	}
	meta["EmissiveAssetPath"] =
		pathTextures.empty() ? (emission ? emission->GetAssetsPath().c_str() : "") : pathTextures[4].c_str();
}

void ResourceMaterial::LoadPaths(Json& meta)
{
	std::string assetPath = meta["DiffuseAssetPath"];
	ModuleResources* resources = App->GetModule<ModuleResources>();

	if (CheckAssetPath(assetPath))
	{
		SetDiffuse(resources->RequestResource<ResourceTexture>(assetPath));
	}

	assetPath = meta["NormalAssetPath"];

	if (CheckAssetPath(assetPath))
	{
		SetNormal(resources->RequestResource<ResourceTexture>(assetPath));
	}

	assetPath = meta["OcclusionAssetPath"];

	if (CheckAssetPath(assetPath))
	{
		SetOcclusion(resources->RequestResource<ResourceTexture>(assetPath));
	}


	switch (loadOptions.shaderType)
	{
		case 0:
			assetPath = meta["MetalicAssetPath"];
			if (CheckAssetPath(assetPath))
			{
				SetMetallic(resources->RequestResource<ResourceTexture>(assetPath));
			}
			break;
		case 1:
			assetPath = meta["SpecularAssetPath"];
			if (CheckAssetPath(assetPath))
			{
				SetSpecular(resources->RequestResource<ResourceTexture>(assetPath));
			}
			break;
	}

	assetPath = meta["EmissiveAssetPath"];

	if (CheckAssetPath(assetPath))
	{
		SetEmission(resources->RequestResource<ResourceTexture>(assetPath));
	}
}

bool ResourceMaterial::CheckAssetPath(std::string& assetPath)
{
	if (assetPath != "")
	{
		bool materialExists = assetPath != "" && App->GetModule<ModuleFileSystem>()->Exists(assetPath.c_str());
		if (materialExists)
		{
			return true;
		}
	}
	return false;
}
