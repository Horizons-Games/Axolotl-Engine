#include "Resources/ResourceModel.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"

ResourceModel::ResourceModel(UID resourceUID, const std::string& fileName, const std::string& assetsPath,
	const std::string& libraryPath) : Resource(resourceUID, fileName, assetsPath, libraryPath),
	options(std::make_shared<OptionsModel>()), numMaterials(0), numMeshes(0)
{
}

ResourceModel::~ResourceModel()
{
	this->Unload();
}

void ResourceModel::InternalLoad()
{
	for (UID materialUID : materialsUIDs)
	{
		std::shared_ptr<Resource> material = App->resources->RequestResource(materialUID).lock();
		if (material)
		{
			material->Load();
		}
	}

	for (UID meshUID : meshesUIDs)
	{
		std::shared_ptr<Resource> mesh = App->resources->RequestResource(meshUID).lock();
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
		std::shared_ptr<Resource> material = App->resources->RequestResource(materialUID).lock();
		if (material)
		{
			material->Unload();
		}
	}

	for (UID meshUID : meshesUIDs)
	{
		std::shared_ptr<Resource> mesh = App->resources->RequestResource(meshUID).lock();
		if (mesh)
		{
			mesh->Unload();
		}
	}
}
