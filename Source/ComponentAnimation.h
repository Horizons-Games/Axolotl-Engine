#pragma once

#include "Components/Component.h"

#define COMPONENT_ANIMATION "Animation"

class ComponentAnimation : public Component
{
public:
	ComponentAnimation(const bool active, GameObject* owner);
	~ComponentAnimation() override;

	void Update() override;

private:
};

