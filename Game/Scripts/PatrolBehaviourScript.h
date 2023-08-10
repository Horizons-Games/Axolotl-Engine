#pragma once

#include "Scripting\Script.h"

// This script performs a generic patrol behaviour between two (or more) waypoints

class ComponentTransform;
class ComponentRigidBody;

class PatrolBehaviourScript : public Script
{
public:
	PatrolBehaviourScript();
	~PatrolBehaviourScript() override = default;

	void Start() override;

	void StartPatrol();
	void Patrolling();

private:

	std::vector<ComponentTransform*> waypointsPatrol;
	int currentWayPoint;
	ComponentTransform* ownerTransform;
	ComponentRigidBody* ownerRigidBody;

	void SetProportionalController() const;
};