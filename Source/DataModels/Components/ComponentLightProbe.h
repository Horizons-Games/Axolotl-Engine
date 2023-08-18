#pragma once
#include "Components/Component.h"


class ComponentLightProbe : public Component
{
public:
	ComponentLightProbe(bool active, GameObject* owner);
	~ComponentLightProbe() override;

	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	bool IsStatic();
	void SetStatic(bool newStatic);
};