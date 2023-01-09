#pragma once
#include "Components/Component.h"

class ComponentMesh : public Component
{
public:
	ComponentMesh(const bool active, GameObject* owner);
	~ComponentMesh() override;

	void Update() override;
	void Draw() override;
	void Display() override;

private:

};