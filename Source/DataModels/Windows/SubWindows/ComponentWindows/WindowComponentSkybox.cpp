#include "StdAfx.h"
#include "WindowComponentSkybox.h"
#include "Components/ComponentSkybox.h"
#include "Resources/ResourceSkyBox.h"

WindowComponentSkybox::WindowComponentSkybox(ComponentSkybox* component) : ComponentWindow("SKYBOX", component)
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
	ImGui::Button("Change Skybox");
}
