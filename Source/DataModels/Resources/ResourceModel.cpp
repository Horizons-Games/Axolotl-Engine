#include "StdAfx.h"

#include "Resources/ResourceModel.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"

#include "DataModels/Resources/ResourceMaterial.h"
#include "DataModels/Resources/ResourceMesh.h"

ResourceModel::ResourceModel(UID resourceUID,
							 const std::string& fileName,
							 const std::string& assetsPath,
							 const std::string& libraryPath) :
	Resource(resourceUID, fileName, assetsPath, libraryPath)
{
}

ResourceModel::~ResourceModel()
{
	this->Unload();
}

void ResourceModel::InternalLoad()
{
	/*for (std::shared_ptr<Resource> material : materials)
	{
		if (material)
		{
			material->Load();
		}
	}

	for (std::shared_ptr<Resource> mesh : meshes)
	{
		if (mesh)
		{
			mesh->Load();
		}
	}*/
}

void ResourceModel::InternalUnload()
{
	/*for (std::shared_ptr<Resource> material : materials)
	{
		if (material)
		{
			material->Unload();
		}
	}

	for (std::shared_ptr<Resource> mesh : meshes)
	{
		if (mesh)
		{
			mesh->Unload();
		}
	}*/
}
