#pragma once
#include "ComponentLight.h"

class ComponentAmbient : public ComponentLight
{
public:
	ComponentAmbient();
	ComponentAmbient(GameObject* parent);
	ComponentAmbient(const float3& color);
	ComponentAmbient(const float3& color, GameObject* parent);
	~ComponentAmbient() {};

	void Draw() override {};
	void Display() override {};
};

