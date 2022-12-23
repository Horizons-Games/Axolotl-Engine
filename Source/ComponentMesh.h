#pragma once
#include "Components/Component.h"

class ComponentMesh : Component
{
public:
	ComponentMesh(const bool active, GameObject* owner);
	~ComponentMesh() override;
	
	void Update() override;

private:

};