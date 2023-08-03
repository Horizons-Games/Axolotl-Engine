#pragma once
#include "Components/Component.h"

class ComponentSkybox : public Component
{
public:
	ComponentSkybox(bool active, GameObject* owner);
	~ComponentSkybox() override;

	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	bool IsStatic();
	void SetStatic(bool newStatic);
};