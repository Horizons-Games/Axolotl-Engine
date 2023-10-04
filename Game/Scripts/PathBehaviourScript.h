#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform;
class ComponentAgent;
class ComponentRigidBody;
class AIMovement;

class PathBehaviourScript : public Script
{
public:
	PathBehaviourScript();
	~PathBehaviourScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;
	
	void StartPath() const;
	void NextPath();
	void ResetPath();

	bool IsPathFinished() const;
	void SetNewPath(GameObject* nPath);

private:
	unsigned int currentWayPoint;
	bool pathFinished;
	float agentVelocity;

	AIMovement* aiMovement;
	ComponentAgent* agentComp;
	ComponentRigidBody* rigidBody;
	std::vector<ComponentTransform*> waypointsPath;
};