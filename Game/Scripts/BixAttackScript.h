#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ModuleInput;

class ComponentAudioSource;
class ComponentTransform;
class ComponentAnimation;

class PlayerManagerScript;

enum class AttackCombo
{
	IDLE,
	FIRST_ATTACK,
	SECOND_ATTACK,
	THIRD_ATTACK
};

class BixAttackScript : public Script
{

public:
	BixAttackScript();
	~BixAttackScript() override = default;

	bool GetIsDeathTouched() const;
	void SetIsDeathTouched(bool isDeathTouch);

private:
	void Start() override;
	void Update(float deltaTime) override;

	void PerformAttack();
	void CheckCombo();

	bool IsAttackAvailable() const;
	void CheckCollision() const;

	float attackCooldown;
	float lastAttackTime;

	bool isDeathTouched;


	ComponentAudioSource* audioSource;
	ComponentTransform* transform;
	ComponentAnimation* animation;
	GameObject* animationGO;

	ModuleInput* input;

	//Provisional
	std::vector<Ray> rays;

	GameObject* ray1GO;
	GameObject* ray2GO;
	GameObject* ray3GO;
	GameObject* ray4GO;

	ComponentTransform* ray1Transform;
	ComponentTransform* ray2Transform;
	ComponentTransform* ray3Transform;
	ComponentTransform* ray4Transform;

	float rayAttackSize;
	//--Provisional

	PlayerManagerScript* playerManager;
	AttackCombo attackComboPhase;
};
