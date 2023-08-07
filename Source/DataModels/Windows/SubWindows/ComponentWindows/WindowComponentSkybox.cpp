#include "StdAfx.h"
#include "WindowComponentSkybox.h"
#include "Components/ComponentSkybox.h"
#include "Resources/ResourceSkyBox.h"
#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowSkyboxInput.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "Skybox/Skybox.h"

WindowComponentSkybox::WindowComponentSkybox(ComponentSkybox* component) : 
	ComponentWindow("SKYBOX", component),
	skyboxInput(std::make_unique<WindowSkyboxInput>())
{
}

WindowComponentSkybox::~WindowComponentSkybox()
{
}

void WindowComponentSkybox::DrawWindowContents()
{
	ComponentSkybox* sky = static_cast<ComponentSkybox*>(component);
	ImGui::Text("Path:");
	ImGui::SameLine();
	ImGui::Text(
		App->GetModule<ModuleScene>()->GetLoadedScene()->GetSkybox()->GetSkyboxResource()->GetAssetsPath().c_str());
	skyboxInput->DrawWindowContents();

	bool useCubemap = sky->GetUseCubeMap();
	ImGui::Text("Use this as Cubemap: ");
	ImGui::SameLine();
	if (ImGui::Checkbox("##UseCubemap", &useCubemap))
	{
		sky->SetUseCubeMap(useCubemap);
	}
	skyboxInput->DrawWindowContents();
}
