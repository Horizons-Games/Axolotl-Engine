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
#include "../Scripts/PlayerMoveScript.h"
#include "../Scripts/EntityDetection.h"
#include "../Scripts/JumpFinisherAttack.h"
#include "../Scripts/JumpFinisherArea.h"
#include "../Scripts/LightFinisherAttackScript.h"
#include "../Scripts/HeavyFinisherAttack.h"

#include "GameObject/GameObject.h"

#include "Auxiliar/Audio/AudioData.h"

#include "../MathGeoLib/Include/Geometry/Ray.h"

#include "debugdraw.h"

#include <set>

#include "AxoLog.h"

REGISTERCLASS(BixAttackScript);

BixAttackScript::BixAttackScript() : Script(), 
	isAttacking(false), attackCooldown(0.6f), attackCooldownCounter(0.f), audioSource(nullptr),
	animation(nullptr), transform(nullptr),
	playerManager(nullptr), attackComboPhase(AttackCombo::IDLE), enemyDetection(nullptr), jumpFinisherScript(nullptr),
	lightFinisherScript(nullptr), normalAttackDistance(0), heavyFinisherAttack(nullptr), bixLightSaber(nullptr)
{
	//REGISTER_FIELD(comboInitTimer, float);
	REGISTER_FIELD(comboCountHeavy, float);
	REGISTER_FIELD(comboCountSoft, float);

	REGISTER_FIELD(attackSoft, float);
	REGISTER_FIELD(attackHeavy, float);
	REGISTER_FIELD(normalAttackDistance, float);

	REGISTER_FIELD(isAttacking, bool);
	//REGISTER_FIELD(attackCooldown, float);

	REGISTER_FIELD(enemyDetection, EntityDetection*);
	REGISTER_FIELD(heavyFinisherAttack, HeavyFinisherAttack*);
	REGISTER_FIELD(bixLightSaber, GameObject*);
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
	lightFinisherScript = owner->GetComponent<LightFinisherAttackScript>();
}

void BixAttackScript::Update(float deltaTime)
{
	// Mark the enemy that is going to be attacked
	UpdateEnemyDetection();

	// Check if the special was activated
	comboSystem->CheckSpecial(deltaTime);

	if (IsAttackAvailable())
	{
		PerformCombos();
	}

	else
	{
		ResetAttackAnimations();
	}
}

void BixAttackScript::UpdateEnemyDetection()
{
	if (comboSystem->NextIsSpecialAttack())
	{
		enemyDetection->UpdateEnemyDetection();
	}

	else
	{
		enemyDetection->UpdateEnemyDetection(normalAttackDistance);
	}
}

void BixAttackScript::PerformCombos()
{
	currentAttack = comboSystem->CheckAttackInput(!playerManager->IsGrounded());
	switch (currentAttack)
	{
		case AttackType::LIGHTNORMAL:
			LOG_DEBUG("NormalAttack Soft");
			LightNormalAttack();
			break;

		case AttackType::HEAVYNORMAL:
			LOG_DEBUG("NormalAttack Heavy");
			HeavyNormalAttack();
			break;

		case AttackType::JUMPNORMAL:
			LOG_DEBUG("NormalAttack Jump");
			JumpNormalAttack();
			break;

		case AttackType::LIGHTFINISHER:
			LOG_DEBUG("Finisher Soft");
			LightFinisher();
			break;

		case AttackType::HEAVYFINISHER:
			LOG_DEBUG("Finisher Heavy");
			HeavyFinisher();
			break;

		case AttackType::JUMPFINISHER:
			LOG_DEBUG("Finisher Jump");
			JumpFinisher();
			break;

		default:
			break;
	}
}

void BixAttackScript::LightNormalAttack()
{
	//Activate visuals and audios
	animation->SetParameter("IsLightAttacking", true);
	audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_SWING);

	//Check collisions and Apply Effects
	GameObject* enemyAttacked = enemyDetection->GetEnemySelected();
	if(enemyAttacked != nullptr)
	{
		LOG_DEBUG("Enemy hit with light attack");
		comboSystem->SuccessfulAttack(static_cast<int>(comboCountSoft), AttackType::LIGHTNORMAL);
		DamageEnemy(enemyAttacked, attackSoft);
	}

	else 
	{
		LOG_DEBUG("Fail light attack");
	}
	isAttacking = true;
}

void BixAttackScript::HeavyNormalAttack()
{
	//Activate visuals and audios
	animation->SetParameter("IsHeavyAttacking", true);
	audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_SWING);

	//Check collisions and Apply Effects
	GameObject* enemyAttacked = enemyDetection->GetEnemySelected();
	if (enemyAttacked != nullptr)
	{
		LOG_DEBUG("Enemy hit with heavy attack");
		comboSystem->SuccessfulAttack(static_cast<int>(comboCountHeavy), AttackType::HEAVYNORMAL);
		DamageEnemy(enemyAttacked, attackHeavy);
	}

	else
	{
		LOG_DEBUG("Fail heavy attack");
	}
	isAttacking = true;
}

void BixAttackScript::JumpNormalAttack()
{
	animation->SetParameter("IsJumpAttacking", true);
	isAttacking = true;

	jumpFinisherScript->PerformGroundSmash(10.0f, 2.0f); // Bix jumping attack
	//jumpFinisherScript->ShootForceBullet(10.0f, 2.0f); // Allura jumping attack, placed it here for now

	comboSystem->SuccessfulAttack(20, AttackType::JUMPNORMAL);
}

void BixAttackScript::LightFinisher()
{
	animation->SetParameter("LightFinisherAttacking", true);
	isAttacking = true;

	lightFinisherScript->ThrowStunItem();

	comboSystem->SuccessfulAttack(-20, AttackType::LIGHTFINISHER);
}

void BixAttackScript::HeavyFinisher()
{
	bixLightSaber->Disable();
	GameObject* enemyAttacked = enemyDetection->GetEnemySelected();
	animation->SetParameter("HeavyFinisherExit", false);
	animation->SetParameter("HeavyFinisherInit", true);
	isAttacking = true;
	if (enemyAttacked != nullptr)
	{
		heavyFinisherAttack->PerformHeavyFinisher(enemyAttacked->GetComponent<ComponentTransform>(), 
			GetOwner()->GetComponent<ComponentTransform>());
		comboSystem->SuccessfulAttack(-50, AttackType::HEAVYFINISHER);
	}
	else
	{
		heavyFinisherAttack->PerformEmptyHeavyFinisher(GetOwner()->GetComponent<ComponentTransform>());
		comboSystem->SuccessfulAttack(-50, AttackType::HEAVYFINISHER);
	}
}

void BixAttackScript::JumpFinisher()
{
	animation->SetParameter("IsJumpAttacking", true);
	isAttacking = true;

	jumpFinisherScript->PerformGroundSmash(15.0f, 4.0f); // Bix jumping finisher
	//jumpFinisherScript->ShootForceBullet(15.0f, 4.0f); // Allura jumping finisher, placed it here for now

	comboSystem->SuccessfulAttack(-35, AttackType::JUMPFINISHER);
}

void BixAttackScript::ResetAttackAnimations()
{
	switch (currentAttack)
	{
		case AttackType::LIGHTNORMAL:
			if (!animation->isPlaying())
			{
				animation->SetParameter("IsLightAttacking", false);
				isAttacking = false;
			}
			break;	

		case AttackType::HEAVYNORMAL:
			if (!animation->isPlaying())
			{
				animation->SetParameter("IsHeavyAttacking", false);
				isAttacking = false;
			}
			break;	

		case AttackType::JUMPNORMAL:
			if (!animation->isPlaying())
			{
				animation->SetParameter("IsJumpAttacking", false);
				isAttacking = false;
			}
			break;	

		case AttackType::LIGHTFINISHER:
			if (!animation->isPlaying())
			{
				animation->SetParameter("LightFinisherAttacking", false);
				isAttacking = false;
			}
			break;	

		case AttackType::HEAVYFINISHER:
			if (!heavyFinisherAttack->IsAttacking()) // Heavy Finisher attack has finished
			{
				animation->SetParameter("HeavyFinisherInit", false);
				animation->SetParameter("HeavyFinisherExit", true);
				bixLightSaber->Enable();
				isAttacking = false;
			}
			break;

		case AttackType::JUMPFINISHER:
			if (!animation->isPlaying())
			{
				animation->SetParameter("IsJumpAttacking", false);
				isAttacking = false;
			}
			break;

		default:
			break;
	}
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

bool BixAttackScript::IsDeathTouched() const
{
	return isDeathTouched;
}

void BixAttackScript::SetIsDeathTouched(bool isDeathTouched)
{
	this->isDeathTouched = isDeathTouched;
}
