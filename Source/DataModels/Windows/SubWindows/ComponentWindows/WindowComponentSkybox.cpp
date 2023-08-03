#include "StdAfx.h"
#include "WindowComponentSkybox.h"
#include "Components/ComponentSkybox.h"
#include "Resources/ResourceSkyBox.h"
#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowSkyboxInput.h"

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
	ImGui::Text(sky->GetSkybox()->GetSkyboxResource()->GetAssetsPath().c_str());
	skyboxInput->DrawWindowContents();
}
