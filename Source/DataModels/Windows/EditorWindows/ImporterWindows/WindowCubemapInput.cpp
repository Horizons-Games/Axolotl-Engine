#include "StdAfx.h"

#include "Cubemap/Cubemap.h"
#include "Defines/ExtensionDefines.h"
#include "ModuleResources.h"
#include "ModuleScene.h"
#include "Resources/ResourceCubemap.h"
#include "Components/ComponentCubemap.h"
#include "Scene/Scene.h"
#include "WindowCubemapInput.h"

WindowCubemapInput::WindowCubemapInput() : WindowFileBrowser()
{
	dialogName = "Change Cubemap";
	title = "Change Cubemap";
	filters = CUBEMAP_EXTENSION;
	startPath = "Assets/Cubemaps";
}
WindowCubemapInput::~WindowCubemapInput()
{
}
void WindowCubemapInput::DoThisIfOk()
{
	this->isLoading = false;
	GameObject* rootObject = App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot();
	std::shared_ptr<ResourceCubemap> resource =
		App->GetModule<ModuleResources>()->RequestResource<ResourceCubemap>(fileDialogImporter.GetFilePathName());
	rootObject->RemoveComponent<ComponentCubemap>();
	ComponentCubemap* cubemapComponent = rootObject->CreateComponent<ComponentCubemap>();
	App->GetModule<ModuleScene>()->GetLoadedScene()->SetCubemap(std::make_unique<Cubemap>(resource));
}