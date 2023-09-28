#include "StdAfx.h"

#include "WindowNavMeshInput.h"

#include "Application.h"
#include "DataModels/Resources/Resource.h"
#include "FileSystem/ModuleResources.h"
#include "Resources/ResourceNavMesh.h"
#include "ModuleNavigation.h"

WindowNavMeshInput::WindowNavMeshInput()
{
	dialogName = "Select NavMesh";
	title = "Load Resource";
	filters = "Resource Files (*.nav){.nav}";
	startPath = "Assets/NavMesh";
}

WindowNavMeshInput::~WindowNavMeshInput()
{
}

void WindowNavMeshInput::DoThisIfOk()
{
	this->isLoading = false;
	std::string filePath = std::string(fileDialogImporter.GetFilePathName());
	std::shared_ptr<ResourceNavMesh> resource =
		App->GetModule<ModuleResources>()->RequestResource<ResourceNavMesh>(filePath);
	App->GetModule<ModuleNavigation>()->SetNavMesh(resource);
}
