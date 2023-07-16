#pragma once

#include "ComponentLight.h"

class Json;

class ComponentDirLight : public ComponentLight
{
public:
	ComponentDirLight();
	ComponentDirLight(GameObject* parent);
	ComponentDirLight(const float3& color, float intensity);
	ComponentDirLight(const float3& color, float intensity, GameObject* parent);
	~ComponentDirLight() override;

	void Draw() const override;

	float2 shadowBias = float2(0.004, 0.0005);

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;
};
