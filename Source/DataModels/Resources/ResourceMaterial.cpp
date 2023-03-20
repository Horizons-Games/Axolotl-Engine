#include "ResourceMaterial.h"

ResourceMaterial::ResourceMaterial(UID resourceUID, const std::string& fileName, const std::string& assetsPath,
	const std::string& libraryPath) : Resource(resourceUID, fileName, assetsPath, libraryPath),
	diffuseUID(0), normalUID(0), occlusionUID(0), specularUID(0),
	options(std::make_shared<OptionsMaterial>()),
	diffuseColor(float3(1.0f, 1.0f, 0.0f)), specularColor(float3(0.5f, 0.3f, 0.5f)),
	shininess(512.0f), normalStrength(1.0f), hasDiffuse(false), hasNormal(false),
	hasOcclusion(false), hasSpecular(false), shininessAlpha(false)
{
}

ResourceMaterial::~ResourceMaterial()
{
	this->Unload();
}
