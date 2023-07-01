#include "StdAfx.h"

#include "ResourceMaterial.h"
#include "FileSystem/Json.h"

ResourceMaterial::ResourceMaterial(UID resourceUID,
								   const std::string& fileName,
								   const std::string& assetsPath,
								   const std::string& libraryPath) :
	Resource(resourceUID, fileName, assetsPath, libraryPath),
	diffuseColor(float4(1.0f, 1.0f, 0.0f, 1.0f)),
	specularColor(float3(0.5f, 0.3f, 0.5f)),
	normalStrength(1.0f),
	smoothness(0.5f),
	metalness(0.0f),
	isTransparent(false),
	shaderType(0),
	tiling(float2(1.0f)),
	offset(float2(0.0f))
{
}

ResourceMaterial::~ResourceMaterial()
{
	this->Unload();
}

void ResourceMaterial::SaveLoadOptions(Json& meta)
{
	meta["diffuseColor_x"] = (float) diffuseColor.x;
	meta["diffuseColor_y"] = (float) diffuseColor.y;
	meta["diffuseColor_z"] = (float) diffuseColor.z;
	meta["diffuseColor_w"] = (float) diffuseColor.w;
	meta["specularColor_x"] = (float) specularColor.x;
	meta["specularColor_y"] = (float) specularColor.y;
	meta["specularColor_z"] = (float) specularColor.z;
	meta["normalStrength"] = (float) normalStrength;
	meta["smoothness"] = (float) smoothness;
	meta["metalness"] = (float) metalness;
	meta["isTransparent"] = (bool) isTransparent;
	meta["tilingx"] = (float) tiling.x;
	meta["tilingy"] = (float) tiling.y;
	meta["offsetx"] = (float) offset.x;
	meta["offsety"] = (float) offset.y;
	meta["shaderType"] = static_cast<int>(loadOptions.shaderType);
}

void ResourceMaterial::LoadLoadOptions(Json& meta)
{
	diffuseColor.x = (float) meta["diffuseColor_x"];
	diffuseColor.y = (float) meta["diffuseColor_y"];
	diffuseColor.z = (float) meta["diffuseColor_z"];
	diffuseColor.w = (float) meta["diffuseColor_w"];
	if (diffuseColor.w == 0.f)
	loadOptions.shaderType = static_cast<int>(meta["shaderType"]);
	{
		diffuseColor.w = 1.0f;
	} 
	specularColor.x = (float) meta["specularColor_x"];
	specularColor.y = (float) meta["specularColor_y"];
	specularColor.z = (float) meta["specularColor_z"];
	normalStrength = (float) meta["normalStrength"];
	smoothness = (float) meta["smoothness"];
	metalness = (float) meta["metalness"];
	isTransparent = (bool) meta["isTransparent"];
	tiling.x = (float) meta["tilingx"];
	tiling.y = (float) meta["tilingy"];
	if (tiling.x == 0.f && tiling.y == 0.f)
	{
		tiling.x = 1.f;
		tiling.y = 1.f;
	}
	offset.x = (float) meta["offsetx"];
	offset.y = (float) meta["offsety"];
}
