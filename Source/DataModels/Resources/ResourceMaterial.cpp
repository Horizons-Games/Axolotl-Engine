#include "ResourceMaterial.h"

ResourceMaterial::ResourceMaterial(UID resourceUID, const std::string& fileName, const std::string& assetsPath,
	const std::string& libraryPath) : Resource(resourceUID, fileName, assetsPath, libraryPath),
	options(std::make_shared<OptionsMaterial>()),
	diffuseColor(float3(1.0f, 1.0f, 0.0f)), specularColor(float3(0.5f, 0.3f, 0.5f)),
	shininess(512.0f), normalStrength(1.0f), shininessAlpha(false)
{
}

ResourceMaterial::~ResourceMaterial()
{
	this->Unload();
}
