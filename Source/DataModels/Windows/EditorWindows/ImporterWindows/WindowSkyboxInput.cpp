#include "StdAfx.h"

#include "ModuleResources.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "Components/ComponentSkybox.h"
#include "Resources/ResourceSkyBox.h"
#include "WindowSkyboxInput.h"
#include "Defines/ExtensionDefines.h"
#include "Skybox/Skybox.h"


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
	rootObject->RemoveComponent<ComponentSkybox>();
	ComponentSkybox* skyboxComponent = rootObject->CreateComponent<ComponentSkybox>();
	App->GetModule<ModuleScene>()->GetLoadedScene()->SetSkybox(std::make_unique<Skybox>(resource));	
	Skybox* sky = App->GetModule<ModuleScene>()->GetLoadedScene()->GetSkybox();
	skyboxComponent->SetSkybox(App->GetModule<ModuleScene>()->GetLoadedScene()->GetSkybox());

		//ImGui::Text(sky->GetSkybox()->GetSkyboxResource()->GetAssetsPath().c_str());
}