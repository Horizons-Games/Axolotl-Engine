#pragma once
#include "Component.h"

#include "Math/float3.h"

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

	bool IsColliding(std::vector<float3>& startingPoints, float3 direction, float size, float stepSize = 0) const;
	void GetMinMaxPoints(std::vector<float3>& startingPoints, std::vector<float3>& points, float stepSize) const;

private:

	bool isTrigger;
};

inline bool ComponentMeshCollider::GetIsTrigger() const
{
	return isTrigger;
}

inline void ComponentMeshCollider::SetIsTrigger(bool newIsTrigger)
{
	isTrigger = newIsTrigger;
}
