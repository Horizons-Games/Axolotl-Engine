#include "ResourceMaterial.h"

ResourceMaterial::ResourceMaterial(UID resourceUID, const std::string& fileName, const std::string& assetsPath,
	const std::string& libraryPath) : Resource(resourceUID, fileName, assetsPath, libraryPath),
	diffuseColor(float3(1.0f, 1.0f, 0.0f)), /*specularColor(float3(0.5f, 0.3f, 0.5f)),
	shininess(512.0f),*/ normalStrength(1.0f), smoothness(0.5f), metalness(0.0f), /*shininessAlpha(false),*/
	hasMetallicAlpha(false)

{
}

ResourceMaterial::~ResourceMaterial()
{
	this->Unload();
}
