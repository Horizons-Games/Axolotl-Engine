#pragma once

#include "ComponentLight.h"

#define COMPONENT_DIRLIGHT "DirLight"

class Json;

class ComponentDirLight : public ComponentLight
{
public:
	ComponentDirLight();
	ComponentDirLight(const std::shared_ptr<GameObject>& parent);
	ComponentDirLight(const float3& color, float intensity);
	ComponentDirLight(const float3& color, float intensity, const std::shared_ptr<GameObject>& parent);
	~ComponentDirLight() {};

	void Draw() override;
	void Display() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;
};
