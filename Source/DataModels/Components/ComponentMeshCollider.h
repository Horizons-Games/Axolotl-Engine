#pragma once
#include "Component.h"

#include "Math/float3.h"
#include "Physics/Physics.h"

enum class Direction
{
	FRONT,
	BACK,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class ComponentMeshCollider : public Component
{
public:
	ComponentMeshCollider(bool active, GameObject* owner);
	~ComponentMeshCollider() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	bool GetIsTrigger() const;
	void SetIsTrigger(bool newIsKinematic);

	bool Move(Direction direction, float distance, float stepSize = 0);
	bool StepsMove(int steps, Direction direction, float distance, RaycastHit& hit, float stepSize = 0);

	bool IsColliding(std::vector<float3>& startingPoints, float3 direction, float size, float stepSize = 0) const;
	void GetMinMaxPoints(const std::vector<float3>& startingPoints, std::vector<float3>& points, float stepSize) const;

private:
	bool isTrigger;

	void
		GetPointsGivenDirection(std::vector<float3>& newPoints, std::vector<float3>& points, Direction direction) const;
	float3 GetMovementGivenDirection(std::vector<float3>& points, Direction direction);
};

inline bool ComponentMeshCollider::GetIsTrigger() const
{
	return isTrigger;
}

inline void ComponentMeshCollider::SetIsTrigger(bool newIsTrigger)
{
	isTrigger = newIsTrigger;
}
