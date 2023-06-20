#include "ResourceCubemap.h"

#include "Application.h"
#include "DataModels/Resources/ResourceTexture.h"
#include "FileSystem/ModuleResources.h"
#include "GL/glew.h"

ResourceCubemap::ResourceCubemap(UID resourceUID,
								 const std::string& fileName,
								 const std::string& assetsPath,
								 const std::string& libraryPath) :
	Resource(resourceUID, fileName, assetsPath, libraryPath)
{
}

ResourceCubemap::~ResourceCubemap()
{
	Unload();
}

void ResourceCubemap::InternalLoad()
{
	hdrTexture->Load();
}

void ResourceCubemap::InternalUnload()
{
	hdrTexture->Unload();
}

bool ResourceCubemap::ChildChanged() const
{
	bool result = false;
	if (hdrTexture && hdrTexture->IsChanged())
	{
		result = true;
		hdrTexture->SetChanged(false);
	}
	return result;
}