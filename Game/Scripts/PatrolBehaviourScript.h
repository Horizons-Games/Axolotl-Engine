#pragma once

#include "Scripting\Script.h"

// This script performs a generic patrol behaviour between two (or more) waypoints

class ComponentTransform;
class ComponentAnimation;
class AIMovement;

class PatrolBehaviourScript : public Script
{
public:
	PatrolBehaviourScript();
	~PatrolBehaviourScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void StartPatrol();
	void StopPatrol();

private:

	ComponentTransform* ownerTransform;
	ComponentAnimation* componentAnimation;
	AIMovement* aiMovement;
	std::vector<ComponentTransform*> waypointsPatrol;
	int currentWayPoint;
	bool patrolStateActivated;
	bool isStoppedAtPatrol;
	float patrolStopDuration;
	float originStopTime;
	std::string patrolAnimationParamater;
	float movementSpeed;
	float rotationSpeed;

	void Patrolling();
	void CheckNextWaypoint();
	void SetProportionalController(float deltaTime) const;
};