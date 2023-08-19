#include "StdAfx.h"
#include "FileSystem/ModuleResources.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "Components/ComponentSkybox.h"
#include "Resources/ResourceSkyBox.h"
#include "WindowSkyboxInput.h"
#include "Defines/ExtensionDefines.h"


WindowSkyboxInput::WindowSkyboxInput() : WindowFileBrowser()
{
	dialogName = "Change Skybox";
	title = "Change Skybox";
	filters = SKYBOX_EXTENSION;
	startPath = "Assets/Skybox";
}
WindowSkyboxInput::~WindowSkyboxInput()
{
}
void WindowSkyboxInput::DoThisIfOk()
{
	this->isLoading = false;
	GameObject* rootObject = App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot();
	std::shared_ptr<ResourceSkyBox> resource =
		App->GetModule<ModuleResources>()->RequestResource<ResourceSkyBox>(fileDialogImporter.GetFilePathName());
	ComponentSkybox * skyboxComponent = rootObject->GetComponentInternal<ComponentSkybox>();
	skyboxComponent->SetSkyboxResource(resource);
}