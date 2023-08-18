#include "StdAfx.h"
#include "WindowComponentSkybox.h"
#include "Resources/ResourceSkyBox.h"
#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowSkyboxInput.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "Components/ComponentSkybox.h"

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
	DrawEnableAndDeleteComponent();
	ComponentSkybox* sky =
		App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot()->GetComponentInternal<ComponentSkybox>();
	if (sky && sky->GetSkyboxResource())
	{
		ImGui::Text("Path:");
		ImGui::SameLine();
		ImGui::Text(sky->GetSkyboxResource()
						->GetAssetsPath()
						.c_str());
	}
	skyboxInput->DrawWindowContents();
}
