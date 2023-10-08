#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "Timer/Timer.h"
#include "Bullet\LinearMath\btVector3.h"

RUNTIME_MODIFIABLE_INCLUDE;

class CameraBossControllerScript;
class CameraControllerScript;
class ComponentAnimation;
class ComponentTransform;
class ComponentTransform2D;
class ComponentRigidBody;
class PlayerManagerScript;
class ModuleInput;
class ModulePlayer;
class ComboManager;

class SwitchPlayerManagerScript : public Script
{
public:
	SwitchPlayerManagerScript();

	void Update(float deltaTime) override;

	void CheckChangeCurrentPlayer();
	void HandleChangeCurrentPlayer();

	void SwitchHealthBars();

	void SetIsSwitchAvailable(bool available);

	void VisualSwitchEffect();
	GameObject* GetSecondPlayer() const;

private:
	void Start() override;

	ModuleInput* input;
	ModulePlayer* modulePlayer;
	Timer changePlayerTimer;
	float currentChangePlayerTime;
	std::vector<float> changingPlayerTime;

	ComboManager* comboSystem;

	GameObject* currentPlayerHealthBar;
	GameObject* secondPlayerHealthBar;
	GameObject* mainCamera;
	CameraControllerScript* camera;
	CameraBossControllerScript* cameraBoss;

	ComponentTransform* currentPlayerTransform;
	ComponentTransform* secondPlayerTransform;

	GameObject* switchPlayersParticlesPrefab;
	GameObject* actualSwitchPlayersParticles;

	ComponentTransform* particlesTransform;

	GameObject* currentPlayer;
	GameObject* secondPlayer;
	PlayerManagerScript* playerManager;
	ComponentTransform* playerTransform;
	ComponentAnimation* componentAnimation;

	btVector3 rigidBodyVec3;

	float3 playerPosition;
	float3 currentHealthBarPosition;
	float3 currentHealthBarScale;
	float3 secondHealthBarPosition;
	float3 secondHealthBarScale;

	ComponentTransform2D* currentHealthBarTransform;
	ComponentTransform2D* secondHealthBarTransform;

	bool isSwitchingHealthBars;
	bool isChangingPlayer;
	bool isNewPlayerEnabled;
	bool isSecondJumpAvailable;

	bool bossScene;
	bool isSwitchAvailable;
	bool isPaused;
};
