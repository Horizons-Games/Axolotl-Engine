#include "Resources/ResourceModel.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"

void ResourceModel::InternalLoad()
{
	for (UID materialUID : materialsUIDs)
	{
		std::shared_ptr<Resource> material = App->GetModuleResources()->RequestResource(materialUID).lock();
		if (material)
		{
			material->Load();
		}
	}

	for (UID meshUID : meshesUIDs)
	{
		std::shared_ptr<Resource> mesh = App->GetModuleResources()->RequestResource(meshUID).lock();
		if (mesh)
		{
			mesh->Load();
		}
	}
}

void ResourceModel::InternalUnload()
{
	for (UID materialUID : materialsUIDs)
	{
		std::shared_ptr<Resource> material = App->GetModuleResources()->RequestResource(materialUID).lock();
		if (material)
		{
			material->Unload();
		}
	}

	for (UID meshUID : meshesUIDs)
	{
		std::shared_ptr<Resource> mesh = App->GetModuleResources()->RequestResource(meshUID).lock();
		if (mesh)
		{
			mesh->Unload();
		}
	}
}
