#include "BixAttackScript.h"

#include "Application.h"

#include "ModuleInput.h"

#include "Physics/Physics.h"

#include "Components/ComponentAudioSource.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentScript.h"

#include "../Scripts/HealthSystem.h"
#include "../Scripts/PlayerManagerScript.h"
#include "../Scripts/PlayerJumpScript.h"
#include "../Scripts/EntityDetection.h"

#include "GameObject/GameObject.h"

#include "Auxiliar/Audio/AudioData.h"

#include "../MathGeoLib/Include/Geometry/Ray.h"

#include "debugdraw.h"

#include <set>

#include "AxoLog.h"
#include "ComboManager.h"

REGISTERCLASS(BixAttackScript);

BixAttackScript::BixAttackScript() : Script(), isAttacking(false), attackCooldown(0.6f), attackCooldownCounter(0.f), audioSource(nullptr),
	animation(nullptr), animationGO(nullptr), transform(nullptr),
	playerManager(nullptr), attackComboPhase(AttackCombo::IDLE), enemyDetection(nullptr), enemyDetectionObject(nullptr)
{
	REGISTER_FIELD(comboInitTimer, float);

	REGISTER_FIELD(comboCountHeavy, float);
	REGISTER_FIELD(comboCountSoft, float);
	REGISTER_FIELD(attackSoft, float);
	REGISTER_FIELD(attackHeavy, float);
	REGISTER_FIELD(isAttacking, bool);
	REGISTER_FIELD(attackCooldown, float);

	REGISTER_FIELD(animationGO, GameObject*);
	REGISTER_FIELD(enemyDetectionObject, GameObject*);
	//--Provisional
}

void BixAttackScript::Start()
{
	audioSource = owner->GetComponent<ComponentAudioSource>();
	transform = owner->GetComponent<ComponentTransform>();
	if (animationGO)
	{
		animation = animationGO->GetComponent<ComponentAnimation>();
	}

	audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_OPEN);
	audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_HUM);

	playerManager = owner->GetComponent<PlayerManagerScript>();

	enemyDetection = enemyDetectionObject->GetComponent<EntityDetection>();

	comboSystem = owner->GetComponent<ComboManager>();
}

void BixAttackScript::Update(float deltaTime)
{
	// Provisional here until we have a way to delay a call to a function a certain time
	// This should go inside the PerformAttack() function but delay setting it to false by 2 seconds or smth like that
	//animation->SetParameter("IsAttacking", false);

	if(isAttacking)
	{
		if (attackCooldownCounter <= 0.0f)
		{
			isAttacking = false;
		}
		else 
		{
			attackCooldownCounter -= deltaTime;
		}
	}

	if (attackComboPhase != AttackCombo::IDLE && !isAttacking)
	{
		if (comboNormalAttackTimer <= 0.0f)
		{
			attackComboPhase = AttackCombo::IDLE;
			if (animation) 
			{
				animation->SetParameter("IsAttacking", false);
				animation->SetParameter("IsAttacking_2", false);
				animation->SetParameter("IsAttacking_3", false);
			}
		}
		else
		{
			comboNormalAttackTimer -= deltaTime;
		}
	}

	comboSystem->CheckSpecial(deltaTime);
	if (IsAttackAvailable())
	{
		AttackType attackType = comboSystem->CheckAttackInput(playerManager->IsJumping());
		switch (attackType)
		{
		case AttackType::SOFTNORMAL:
			NormalAttack(false);
			break;
		case AttackType::HEAVYNORMAL:
			NormalAttack(true);
			break;
		case AttackType::JUMPATTACK:
			JumpAttack();
			break;
		case AttackType::SOFTFINISHER:
			SoftFinisher();
			break;
		case AttackType::HEAVYFINISHER:
			HeavyFinisher();
			break;
		default:
			break;
		}
	}
}

void BixAttackScript::NormalAttack(bool heavy) 
{
	//Activate visuals and audios
	ActivateAnimationCombo();
	audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_SWING);

	//Check collisions and Apply Effects
	GameObject* enemyAttacked = enemyDetection->GetEnemySelected();
	if(enemyAttacked != nullptr)
	{
		int comboCount = heavy ? comboCountHeavy : comboCountSoft;
		float attack = heavy ? attackHeavy : attackSoft;
		comboSystem->SuccessfulAttack(comboCount, heavy);
		DamageEnemy(enemyAttacked, attack);
	}
	isAttacking = true;
	attackCooldownCounter = attackCooldown;
}

void BixAttackScript::JumpAttack()
{
}

void BixAttackScript::SoftFinisher()
{
}

void BixAttackScript::HeavyFinisher()
{
}

void BixAttackScript::DamageEnemy(GameObject* enemyAttacked, float damageAttack) 
{
	if (enemyAttacked != nullptr)
	{
		HealthSystem* healthScript = enemyAttacked->GetComponent<HealthSystem>();
		float attack = isDeathTouched ? healthScript->GetMaxHealth() : damageAttack;
		healthScript->TakeDamage(attack);
		audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_CLASH);
	}
}

bool BixAttackScript::IsAttackAvailable() const
{
	return !isAttacking;
}

void BixAttackScript::ActivateAnimationCombo()
{
	// Attack, starting the combo
	if (animation) 
	{
		switch (attackComboPhase)
		{
		case AttackCombo::IDLE:
			attackComboPhase = AttackCombo::FIRST_ATTACK;
			animation->SetParameter("IsAttacking", true);
			comboNormalAttackTimer = 0.2f;
			break;
		case AttackCombo::FIRST_ATTACK:
			attackComboPhase = AttackCombo::SECOND_ATTACK;
			animation->SetParameter("IsAttacking", false);
			animation->SetParameter("IsAttacking_2", true);
			comboNormalAttackTimer = 0.2f;
			break;
		case AttackCombo::SECOND_ATTACK:
			attackComboPhase = AttackCombo::THIRD_ATTACK;
			animation->SetParameter("IsAttacking_2", false);
			animation->SetParameter("IsAttacking_3", true);
			comboNormalAttackTimer = 0.0f;
			break;
		default:
			break;
		}
	}
}

bool BixAttackScript::GetIsDeathTouched() const
{
	return isDeathTouched;
}

void BixAttackScript::SetIsDeathTouched(bool isDeathTouched)
{
	this->isDeathTouched = isDeathTouched;
}
