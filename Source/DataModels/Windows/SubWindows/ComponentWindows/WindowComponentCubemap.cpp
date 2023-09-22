#include "StdAfx.h"

#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowCubemapInput.h"
#include "WindowComponentCubemap.h"
#include "Components/ComponentCubemap.h"
#include "Resources/ResourceCubemap.h"
#include "ModuleScene.h"
#include "Application.h"
#include "Scene/Scene.h"
#include "Cubemap/Cubemap.h"


WindowComponentCubemap::WindowComponentCubemap(ComponentCubemap* component) : 
	ComponentWindow("CUBEMAP", component),
	cubemapInput(std::make_unique<WindowCubemapInput>())
{
}

WindowComponentCubemap::~WindowComponentCubemap()
{
}

void WindowComponentCubemap::DrawWindowContents()
{
	ComponentCubemap* asCubemap = static_cast<ComponentCubemap*>(component);

	if (asCubemap)
	{
		ImGui::Text("Path:");
		ImGui::SameLine();
		ImGui::Text(
			App->GetModule<ModuleScene>()->GetLoadedScene()->GetCubemap()->GetCubemapResource()->GetAssetsPath().c_str());
		cubemapInput->DrawWindowContents();

		ImGui::Text("");

		const ModuleScene* scene = App->GetModule<ModuleScene>();
		const Scene* loadedScene = scene->GetLoadedScene();
		Cubemap* currentCubemap = loadedScene->GetCubemap();

		float intensity = currentCubemap->GetIntensity();
		ImGui::Dummy(ImVec2(5.0f, 0.0f));
		ImGui::Text("Intensity:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60.0f);
		if (ImGui::DragFloat("##Intensity", &intensity, 0.01f, 0.0f, max_intensity))
		{
			if (intensity > max_intensity)
			{
				intensity = max_intensity;
			}
			else if (intensity < 0.0f)
			{
				intensity = 0.0f;
			}

			currentCubemap->SetIntensity(intensity);
		}
		
	}
}
