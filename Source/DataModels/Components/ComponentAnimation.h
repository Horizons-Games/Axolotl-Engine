#pragma once

#include "Components/Component.h"

class ComponentAnimation : public Component
{
public:
	ComponentAnimation(const bool active, GameObject* owner);
	~ComponentAnimation() override;

	void Update() override;
	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

private:
};

