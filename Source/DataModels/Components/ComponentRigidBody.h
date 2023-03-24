#pragma once
#include "Component.h"

class ComponentRigidBody :
	public Component
{
public:
	ComponentRigidBody(bool active, GameObject* owner);
	~ComponentRigidBody() override;

	void Update() override;
	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;
};

