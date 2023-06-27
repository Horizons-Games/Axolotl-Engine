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

	void RemoveAgentFromCrowd();

	float GetMaxSpeed() const;
	float GetMaxAcceleration() const;
	float3 GetTargetPosition() const;
	bool IsAvoidingObstacle() const;

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	unsigned int targetPolygon = 0;		  // Target Polygon of the NavMesh to navigate
	float3 targetPosition = float3::zero; // Target position of the NavMesh to navigate
	int agentId = -1;					  // Agent identifier in NavMesh's crowd

	float maxSpeed = 5.0f;
	float maxAcceleration = 8.0f;
	bool avoidingObstacle = true;
	bool shouldAddAgentToCrowd = true;

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