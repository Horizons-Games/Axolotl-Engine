#include "WindowComponentCubeMap.h"
#include "ComponentCubeMap.h"

WindowComponentCubeMap::WindowComponentCubeMap(ComponentCubeMap* component) : ComponentWindow("Cube Map", component)
{
}

WindowComponentCubeMap::~WindowComponentCubeMap()
{
}

void WindowComponentCubeMap::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ComponentCubeMap* ascubeMap = static_cast<ComponentCubeMap*>(component);

	if (ascubeMap)
	{
		ImGui::Text("");
		float intensity = ascubeMap->GetIntensity();
		float max_intensity = 1.0;
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

			ascubeMap->SetIntensity(intensity);
		}
		
	}
}
