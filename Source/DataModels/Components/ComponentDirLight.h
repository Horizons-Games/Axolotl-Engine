#pragma once

#include "ComponentLight.h"

#define COMPONENT_DIRLIGHT "DirLight"

class Json;

class ComponentDirLight : public ComponentLight
{
public:
	ComponentDirLight();
	ComponentDirLight(ComponentDirLight& componentDirLight);
	ComponentDirLight(GameObject* parent);
	ComponentDirLight(const float3& color, float intensity);
	ComponentDirLight(const float3& color, float intensity, GameObject* parent);
	~ComponentDirLight() override;

	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;
};
