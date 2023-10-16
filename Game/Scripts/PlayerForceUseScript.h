#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ModuleInput;
class PlayerManagerScript;
class PlayerMoveScript;
class ComponentTransform;
class ComponentRigidBody;
class ComponentAnimation;
class ComponentAudioSource;

class PlayerForceUseScript : public Script
{
public:
    PlayerForceUseScript();
	~PlayerForceUseScript() override = default;

    void Start() override;
    void Update(float deltaTime) override;

	bool IsForceActive() const;

private:
	void DisableAllInteractions() const;
	void EnableAllInteractions() const;
	void InitForce();
	void FinishForce();

    GameObject* gameObjectAttached;
	float distancePointGameObjectAttached;
	float maxDistanceForce;
	float minDistanceForce;
	float lastHorizontalSensitivity;
	float lastVerticalSensitivity;
	float gravity;
	bool isForceActive;
	bool objectStaticness;

    std::string tag;
	
	PlayerManagerScript* playerManager;
	PlayerMoveScript* moveScript;

	ComponentAudioSource* componentAudioSource;
	ComponentAnimation* componentAnimation;
	ComponentTransform* transform;
	ComponentRigidBody* rigidBody;

	ModuleInput* input;
};
