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
	currentAttack = comboSystem->CheckAttackInput(!playerManager->isGrounded());
	switch (currentAttack)
	{
		case AttackType::SOFTNORMAL:
			LOG_DEBUG("NormalAttack Soft");
			NormalAttack();
			break;

		case AttackType::HEAVYNORMAL:
			LOG_DEBUG("NormalAttack Heavy");
			NormalAttack(); // This should be a different kind of attack (a "heavy" one)
			break;

		case AttackType::JUMPNORMAL:
			LOG_DEBUG("JumpAttack");
			NormalJumpAttack();
			break;

		case AttackType::LIGHTFINISHER:
			LOG_DEBUG("Special Soft");
			LightFinisher();
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

void BixAttackScript::NormalAttack() 
{
	//Activate visuals and audios
	//ActivateAnimationCombo(); // TODO: Fix this animation system
	animation->SetParameter("IsAttacking", true);
	audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_SWING);

	//Check collisions and Apply Effects
	GameObject* enemyAttacked = enemyDetection->GetEnemySelected();
	if(enemyAttacked != nullptr)
	{
		bool heavy = (currentAttack == AttackType::HEAVYNORMAL);
		LOG_DEBUG("Enemy hitted");
		float comboCount = heavy ? comboCountHeavy : comboCountSoft;
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
	animation->SetParameter("IsJumpAttacking", true);
	isAttacking = true;

	jumpFinisherScript->PerformGroundSmash(10.0f, 2.0f); // Bix jumping attack
	//jumpFinisherScript->ShootForceBullet(10.0f, 2.0f); // Allura jumping attack, placed it here for now

	comboSystem->SuccessfulAttack(20, AttackType::JUMPNORMAL);
}

void BixAttackScript::LightFinisher()
{
	lightFinisherScript->ThrowStunItem();
	isAttacking = true;

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
	jumpFinisherScript->PerformGroundSmash(15.0f, 4.0f); // Bix jumping finisher
	//jumpFinisherScript->ShootForceBullet(15.0f, 4.0f); // Allura jumping finisher, placed it here for now

	comboSystem->SuccessfulAttack(-35, AttackType::HEAVYFINISHER);
}

void BixAttackScript::ResetAttackAnimations()
{
	switch (currentAttack)
	{
		case AttackType::SOFTNORMAL:
			if (!animation->isPlaying())
			{
				animation->SetParameter("IsAttacking", false);
				isAttacking = false;
			}
			break;	

		case AttackType::HEAVYNORMAL:
			if (!animation->isPlaying())
			{
				animation->SetParameter("IsAttacking", false);
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
			isAttacking = false;
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
			isAttacking = false;
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
