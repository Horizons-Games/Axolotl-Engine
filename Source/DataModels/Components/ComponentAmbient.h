#pragma once
#include "ComponentLight.h"

#define COMPONENT_AMBIENT "Ambient"

class Json;

class ComponentAmbient : public ComponentLight
{
public:
	ComponentAmbient();
	ComponentAmbient(const std::shared_ptr<GameObject>& parent);
	ComponentAmbient(const float3& color);
	ComponentAmbient(const float3& color, const std::shared_ptr<GameObject>& parent);
	~ComponentAmbient() {};

	void Draw() override {};
	void Display() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;
};

