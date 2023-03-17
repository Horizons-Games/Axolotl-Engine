#pragma once
#include "ComponentLight.h"

#define COMPONENT_AMBIENT "Ambient"

class Json;

class ComponentAmbient : public ComponentLight
{
public:
	ComponentAmbient();
	ComponentAmbient(ComponentAmbient& componentAmbient);
	ComponentAmbient(GameObject* parent);
	ComponentAmbient(const float3& color);
	ComponentAmbient(const float3& color, GameObject* parent);
	~ComponentAmbient() override;

	void Draw() override {};

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;
};

