#pragma once

#include "Scripting\Script.h"

// This script performs a seeking behaviour once the player enters in range

class ComponentTransform;
class AIMovement;
class ModulePlayer;

class SeekBehaviourScript : public Script
{
public:
	SeekBehaviourScript();
	~SeekBehaviourScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void Seeking();
	void DisableMovement();
	void DisableRotation();
	void RotateToTarget();

	GameObject* GetTarget() const;

private:
	GameObject* target;
	ModulePlayer* modulePlayer;

	ComponentTransform* targetTransform;
	ComponentTransform* ownerTransform;
	AIMovement* aiMovement;
};
