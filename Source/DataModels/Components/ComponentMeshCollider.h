#pragma once
#include "Component.h"

class ComponentMeshCollider :
	public Component
{
public:
	ComponentMeshCollider(bool active, GameObject* owner);
	~ComponentMeshCollider() override;

	void Update() override;
	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	bool GetIsTrigger() const;
	void SetIsTrigger(bool newIsKinematic);

private:

	bool isTrigger;
};

inline bool ComponentMeshCollider::GetIsTrigger() const
{
	return this->isTrigger;
}

inline void ComponentMeshCollider::SetIsTrigger(bool newIsTrigger)
{
	isTrigger = newIsTrigger;
}
