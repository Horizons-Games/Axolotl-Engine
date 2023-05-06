#include "ResourceMaterial.h"
#include "FileSystem/Json.h"

ResourceMaterial::ResourceMaterial(UID resourceUID, const std::string& fileName, const std::string& assetsPath,
	const std::string& libraryPath) : Resource(resourceUID, fileName, assetsPath, libraryPath),
	diffuseColor(float4(1.0f, 1.0f, 0.0f, 1.0f)), specularColor(float3(0.5f, 0.3f, 0.5f)),
	normalStrength(1.0f), smoothness(0.5f), metalness(0.0f), isTransparent(false)
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
	meta["specularColor_x"] = (float) specularColor.x;
	meta["specularColor_y"] = (float) specularColor.y;
	meta["specularColor_z"] = (float) specularColor.z;
	meta["normalStrength"] = (float) normalStrength;
	meta["smoothness"] = (float) smoothness;
	meta["metalness"] = (float) metalness;
}

void ResourceMaterial::LoadLoadOptions(Json& meta)
{
	diffuseColor.x = (float) meta["diffuseColor_x"];
	diffuseColor.y = (float) meta["diffuseColor_y"];
	diffuseColor.z = (float) meta["diffuseColor_z"];
	specularColor.x = (float) meta["specularColor_x"];
	specularColor.y = (float) meta["specularColor_y"];
	specularColor.z = (float) meta["specularColor_z"];
	normalStrength = (float) meta["normalStrength"];
	smoothness = (float) meta["smoothness"];
	metalness = (float) meta["metalness"];
}
