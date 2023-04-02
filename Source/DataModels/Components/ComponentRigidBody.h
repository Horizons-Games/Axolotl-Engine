#pragma once
#include "Component.h"

#include "Math/float3.h"

class ComponentRigidBody : public Component
{
public:
	ComponentRigidBody(bool active, GameObject* owner);
	~ComponentRigidBody() override;

	void Update() override;
	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	bool GetIsKinematic() const;
	void SetIsKinematic(bool newIsKinematic);

private:
	bool isKinematic;
	float m;
	float g;
	float3 v0;
};

inline bool ComponentRigidBody::GetIsKinematic() const
{
	return this->isKinematic;
}

inline void ComponentRigidBody::SetIsKinematic(bool newIsKinematic)
{
	isKinematic = newIsKinematic;
}
