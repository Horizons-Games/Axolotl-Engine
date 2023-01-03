#pragma once
#include "Components/Component.h"
#include "3DModels/Mesh.h"

class ComponentMesh : public Component
{
public:
	ComponentMesh(const bool active, GameObject* owner);
	~ComponentMesh() override;
	
	void Update() override;
	void OnEditorUpdate() override;

	Mesh* mesh = nullptr;

private:

};