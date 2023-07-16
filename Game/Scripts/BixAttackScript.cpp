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

BixAttackScript::BixAttackScript() : Script(), attackCooldown(0.6f), lastAttackTime(0.f), audioSource(nullptr),
	animation(nullptr), animationGO(nullptr), transform(nullptr),
	playerManager(nullptr), attackComboPhase(AttackCombo::IDLE), enemyDetection(nullptr), enemyDetectionObject(nullptr)
{
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
	animation->SetParameter("IsAttacking", false);

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
	//Activate visuals
	animation->SetParameter("IsAttacking", true);

	//Check collisions and Apply Effects
	lastAttackTime = SDL_GetTicks() / 1000.0f;
	audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_SWING);
	GameObject* enemyAttacked = enemyDetection->GetEnemySelected();
	comboSystem->SuccessfulAttack(20, heavy);
	if(enemyAttacked != nullptr) //We need a form to CHANGE this check collision because this is really static
	{
		//If Collisions call combo again to update the combo count
		comboSystem->SuccessfulAttack(20, heavy);
	}
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

void BixAttackScript::PerformAttack()
{
	lastAttackTime = SDL_GetTicks() / 1000.0f;

	audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_SWING);

	CheckCollision();
}

void BixAttackScript::CheckCollision() const
{
	GameObject* enemyAttacked = enemyDetection->GetEnemySelected();

	if (enemyAttacked != nullptr)
	{
		HealthSystem* healthScript = enemyAttacked->GetComponent<HealthSystem>();
		float damageAttack = playerManager->GetPlayerAttack();
		if (!isDeathTouched)
		{
			healthScript->TakeDamage(damageAttack);
			audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_CLASH);
		}
		else
		{
			healthScript->TakeDamage(healthScript->GetMaxHealth());
			audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_CLASH);
		}
	}
}

bool BixAttackScript::IsAttackAvailable() const
{
	return (SDL_GetTicks() / 1000.0f > lastAttackTime + attackCooldown);
}

void BixAttackScript::CheckCombo()
{
	// Attack, starting the combo

		if (animation && attackComboPhase == AttackCombo::IDLE)
		{
			attackComboPhase = AttackCombo::FIRST_ATTACK;
			animation->SetParameter("IsAttacking", true);

			PerformAttack();
		}

	// Attack, continue the combo
		//LOG_VERBOSE("KEEP Pressing left mouse button");

		if (animation && attackComboPhase == AttackCombo::FIRST_ATTACK)
		{
			attackComboPhase = AttackCombo::SECOND_ATTACK;
			animation->SetParameter("IsAttacking_2", true);

			PerformAttack();
		}

		else if (animation && attackComboPhase == AttackCombo::SECOND_ATTACK)
		{
			attackComboPhase = AttackCombo::THIRD_ATTACK;
			animation->SetParameter("IsAttacking_3", true);

			PerformAttack();
		}

		else if (animation && attackComboPhase == AttackCombo::THIRD_ATTACK)
		{
			attackComboPhase = AttackCombo::IDLE;
			animation->SetParameter("IsAttacking", false);

			PerformAttack();
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
