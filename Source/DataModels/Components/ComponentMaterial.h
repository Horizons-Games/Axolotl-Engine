#pragma once
#include "Components/Component.h"

class ComponentMaterial : public Component
{
public:
	ComponentMaterial(bool active, GameObject* owner);
	~ComponentMaterial() override;

	void Update() override;
	void Display() override;

private:

};