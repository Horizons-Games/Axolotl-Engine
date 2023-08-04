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
#include "../Scripts/JumpFinisherAttack.h"
#include "../Scripts/JumpFinisherArea.h"

#include "GameObject/GameObject.h"

#include "Auxiliar/Audio/AudioData.h"

#include "../MathGeoLib/Include/Geometry/Ray.h"

#include "debugdraw.h"

#include <set>

#include "AxoLog.h"
#include "ComboManager.h"

REGISTERCLASS(BixAttackScript);

BixAttackScript::BixAttackScript() : Script(), 
	isAttacking(false), attackCooldown(0.6f), attackCooldownCounter(0.f), audioSource(nullptr),
	animation(nullptr), transform(nullptr),
	playerManager(nullptr), attackComboPhase(AttackCombo::IDLE), enemyDetection(nullptr), jumpFinisherScript(nullptr)
{
	//REGISTER_FIELD(comboInitTimer, float);
	REGISTER_FIELD(comboCountHeavy, float);
	REGISTER_FIELD(comboCountSoft, float);

	REGISTER_FIELD(attackSoft, float);
	REGISTER_FIELD(attackHeavy, float);
	REGISTER_FIELD(isAttacking, bool);
	//REGISTER_FIELD(attackCooldown, float);

	REGISTER_FIELD(enemyDetection, EntityDetection*);
}

void BixAttackScript::Start()
{
	audioSource = owner->GetComponent<ComponentAudioSource>();
	transform = owner->GetComponent<ComponentTransform>();
	animation = owner->GetComponent<ComponentAnimation>();

	audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_OPEN);
	audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_HUM);

	playerManager = owner->GetComponent<PlayerManagerScript>();
	comboSystem = owner->GetComponent<ComboManager>();
	jumpFinisherScript = owner->GetComponent<JumpFinisherAttack>();
}

void BixAttackScript::Update(float deltaTime)
{
	// Provisional here until we have a way to delay a call to a function a certain time
	// This should go inside the PerformAttack() function but delay setting it to false by 2 seconds or smth like that
	if (isAttacking) 
	{
		if (animation && !animation->isPlaying()) 
		{
			isAttacking = false;
			animation->SetParameter("IsAttacking", false);
		}
	}

	/*if(isAttacking)
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
	}*/

	comboSystem->CheckSpecial(deltaTime);
	if (IsAttackAvailable())
	{
		AttackType attackType = comboSystem->CheckAttackInput(!playerManager->isGrounded());
		switch (attackType)
		{
			case AttackType::SOFTNORMAL:
				LOG_DEBUG("NormalAttack Soft");
				NormalAttack(false);
				break;

			case AttackType::HEAVYNORMAL:
				LOG_DEBUG("NormalAttack Heavy");
				NormalAttack(true); // This should be a different kind of attack
				break;

			case AttackType::JUMPNORMAL:
				LOG_DEBUG("JumpAttack");
				NormalJumpAttack();
				break;

			case AttackType::SOFTFINISHER:
				LOG_DEBUG("Special Soft");
				SoftFinisher();
				break;

			case AttackType::HEAVYFINISHER:
				LOG_DEBUG("Special Heavy");
				HeavyFinisher();
				break;

			case AttackType::JUMPFINISHER:
				LOG_DEBUG("Special Jump");
				JumpFinisher();
				break;

			default:
				break;
		}
	}
}

void BixAttackScript::NormalAttack(bool heavy) 
{
	//Activate visuals and audios
	//ActivateAnimationCombo(); // TODO: Fix this animation system
	animation->SetParameter("IsAttacking", true);
	audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_SWING);

	//Check collisions and Apply Effects
	GameObject* enemyAttacked = enemyDetection->GetEnemySelected();
	if(enemyAttacked != nullptr)
	{
		LOG_DEBUG("Enemy hitted");
		int comboCount = heavy ? comboCountHeavy : comboCountSoft;
		float attack = heavy ? attackHeavy : attackSoft;
		AttackType type = heavy ? AttackType::HEAVYNORMAL : AttackType::SOFTNORMAL;
		comboSystem->SuccessfulAttack(comboCount, type);
		DamageEnemy(enemyAttacked, attack);
	}

	else 
	{
		LOG_DEBUG("Fail attack");
	}
	isAttacking = true;
	//attackCooldownCounter = attackCooldown;
}

void BixAttackScript::NormalJumpAttack()
{
	jumpFinisherScript->PerformGroundSmash(); // Bix jumping attack
	//jumpFinisherScript->ShootForceBullet(); // Allura jumping attack, placed it here for now

	comboSystem->SuccessfulAttack(20, AttackType::JUMPNORMAL);
}

void BixAttackScript::SoftFinisher()
{
	comboSystem->SuccessfulAttack(-20, AttackType::SOFTFINISHER);

	LOG_VERBOSE("SOFT FINISHER!!!!!!");
}

void BixAttackScript::HeavyFinisher()
{
	comboSystem->SuccessfulAttack(-50, AttackType::HEAVYFINISHER);

	LOG_VERBOSE("HEAVY FINISHER!!!!!!");
}

void BixAttackScript::JumpFinisher()
{
	jumpFinisherScript->PerformGroundSmash(); // Bix jumping finisher
	//jumpFinisherScript->ShootForceBullet(); // Allura jumping finisher, placed it here for now

	comboSystem->SuccessfulAttack(-35, AttackType::HEAVYFINISHER);

	LOG_VERBOSE("JUMP FINISHER!!!!!!");
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

bool BixAttackScript::GetIsDeathTouched() const
{
	return isDeathTouched;
}

void BixAttackScript::SetIsDeathTouched(bool isDeathTouched)
{
	this->isDeathTouched = isDeathTouched;
}
