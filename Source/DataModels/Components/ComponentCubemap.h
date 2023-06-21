#pragma once
#include "Components/Component.h"


class ComponentCubemap : public Component
{
public:
	ComponentCubemap(bool active, GameObject* owner);
	~ComponentCubemap() override;

	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	bool IsStatic();
	void SetStatic(bool newStatic);
};