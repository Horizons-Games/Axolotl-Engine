#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform;
class AIMovement;

class PathBehaviourScript : public Script
{
public:
	PathBehaviourScript();
	~PathBehaviourScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;
	
	void StartPath();
	void NextPath();
	bool IsPathFinished() const;

private:
	unsigned int currentWayPoint;
	bool pathFinished;

	AIMovement* aiMovement;
	std::vector<ComponentTransform*> waypointsPath;
	ComponentTransform* ownerTransform;
};