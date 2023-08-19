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

	void Patrolling();

private:
	std::vector<GameObject*> waypoints;
	std::vector<ComponentTransform*> transformWaypoints;
	int currentWaypoint;

	ComponentTransform* currentWayPointTransform;
	ComponentTransform* ownerTransform;
	ComponentRigidBody* ownerRigidBody;

	void SetProportionalController() const;
};