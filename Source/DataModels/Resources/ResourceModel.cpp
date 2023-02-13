#include "Resources/ResourceModel.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"

void ResourceModel::InternalLoad()
{
	for (std::shared_ptr<Resource> material : materials)
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
	}
}

void ResourceModel::InternalUnload()
{
	for (std::shared_ptr<Resource> material : materials)
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
	}
}
