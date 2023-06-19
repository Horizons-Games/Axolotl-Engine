#pragma once
#include "Components/Component.h"


class ComponentCubemap : public Component
{
public:
	ComponentCubemap(bool active, GameObject* owner);
	~ComponentCubemap() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	bool IsStatic();
	void SetStatic(bool newStatic);
};