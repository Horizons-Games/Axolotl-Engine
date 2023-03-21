#include "ResourceMaterial.h"

ResourceMaterial::ResourceMaterial(UID resourceUID, const std::string& fileName, const std::string& assetsPath,
	const std::string& libraryPath) : Resource(resourceUID, fileName, assetsPath, libraryPath),
	diffuseColor(float3(1.0f, 1.0f, 1.0f)), specularColor(float3(0.0f, 0.0f, 0.0f)),
	shininess(0.1f), normalStrength(1.0f), shininessAlpha(false)
{
}

ResourceMaterial::~ResourceMaterial()
{
	this->Unload();
}
