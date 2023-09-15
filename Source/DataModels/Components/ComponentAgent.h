#pragma once
#include "Auxiliar/Generics/Updatable.h"
#include "Component.h"
#include "Math/float3.h"

class ComponentAgent: public Component, public Updatable
{
public:
	ComponentAgent(bool active, GameObject* owner);
	~ComponentAgent();

	void Update() override;

	void SetMoveTarget(float3 newTargetPosition, bool usePathfinding = true);
	void SetMaxSpeed(float newSpeed);
	void SetInitialMaxSpeed(float newSpeed);
	void SetMaxAcceleration(float newAcceleration);
	void SetInitialMaxAcceleration(float newAcceleration);
	void SetAgentObstacleAvoidance(bool avoidanceActive);
	void SetUpdateRigidBody(bool newUpdateRigidBody);
	void SetYOffset(float newYOffset);
	void SetEnableRotation(bool newEnabledToRotate);
	void SetTargetPositionRotate(float3 newTargetPositionRotate);
	void SetRotationSpeed(float newRotationVelocity);
	void AddAgentToCrowd();
	void RemoveAgentFromCrowd();

	float3 GetVelocity() const;
	float GetMaxSpeed() const;
	float GetInitialMaxSpeed() const;
	float GetMaxAcceleration() const;
	float GetInitialMaxAcceleration() const;
	float3 GetTargetPosition() const;
	bool GetUpdateRigidBody() const;
	float GetYOffset() const;
	bool GetEnableRotation() const;
	float3 GetTargetPositionRotate() const;
	float GetRotationSpeed() const;
	bool IsAvoidingObstacle() const;

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;
	Quat CalculateRotationToPosition();

	unsigned int targetPolygon = 0;		  // Target Polygon of the NavMesh to navigate
	float3 targetPosition = float3::zero; // Target position of the NavMesh to navigate
	float3 targetPositionRotate = float3::zero; // Position to which we want to rotate
	int agentId = -1;					  // Agent identifier in NavMesh's crowd

	float maxSpeed = 5.0f;
	float initialMaxSpeed = maxSpeed;
	float maxAcceleration = 8.0f;
	float initialMaxAcceleration = maxAcceleration;
	float yOffset = 0.0f;
	float rotationSpeed = 3.0f;
	bool avoidingObstacle = true;
	bool shouldAddAgentToCrowd = true;
	bool updateRigidBody = false;
	bool enabledToRotate = true;
	bool isFirstAdded = true;

	ComponentTransform* transform;

};

inline float ComponentAgent::GetMaxSpeed() const
{
	return maxSpeed;
}

inline float ComponentAgent::GetMaxAcceleration() const
{
	return maxAcceleration;
}

inline float3 ComponentAgent::GetTargetPosition() const
{
	return targetPosition;
}

inline bool ComponentAgent::IsAvoidingObstacle() const
{
	return avoidingObstacle;
}

inline bool ComponentAgent::GetUpdateRigidBody() const
{
	return updateRigidBody;
}

inline void ComponentAgent::SetUpdateRigidBody(bool newUpdateRigidBody)
{
	updateRigidBody = newUpdateRigidBody;
}

inline float ComponentAgent::GetYOffset() const
{
	return yOffset;
}

inline void ComponentAgent::SetYOffset(float newYOffset)
{
	yOffset = newYOffset;
}

inline bool ComponentAgent::GetEnableRotation() const
{
	return enabledToRotate;
}

inline void ComponentAgent::SetEnableRotation(bool newEnabledToRotate)
{
	enabledToRotate = newEnabledToRotate;
}

inline float3 ComponentAgent::GetTargetPositionRotate() const
{
	return targetPositionRotate;
}

inline void ComponentAgent::SetTargetPositionRotate(float3 newTargetPositionRotate)
{
	targetPositionRotate = newTargetPositionRotate;
}

inline float ComponentAgent::GetInitialMaxSpeed() const
{
	return initialMaxSpeed;
}

inline void ComponentAgent::SetInitialMaxSpeed(float newSpeed)
{
	initialMaxSpeed = newSpeed;
}

inline float ComponentAgent::GetInitialMaxAcceleration() const
{
	return initialMaxAcceleration;
}

inline void ComponentAgent::SetInitialMaxAcceleration(float newAcceleration)
{
	initialMaxAcceleration = newAcceleration;
}

inline float ComponentAgent::GetRotationSpeed() const
{
	return rotationSpeed;
}

inline void ComponentAgent::SetRotationSpeed(float newRotationSpeed)
{
	rotationSpeed = newRotationSpeed;
}