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
#include "../Scripts/EntityDetection.h"

#include "GameObject/GameObject.h"

#include "Auxiliar/Audio/AudioData.h"

#include "../MathGeoLib/Include/Geometry/Ray.h"

#include "debugdraw.h"

#include <set>

#include "AxoLog.h"

REGISTERCLASS(BixAttackScript);

BixAttackScript::BixAttackScript() : Script(), attackCooldown(0.6f), lastAttackTime(0.f), audioSource(nullptr),
	input(nullptr), animation(nullptr), animationGO(nullptr), transform(nullptr),
	playerManager(nullptr), attackComboPhase(AttackCombo::IDLE), enemyDetection(nullptr), enemyDetectionObject(nullptr)
{
	REGISTER_FIELD(attackCooldown, float);
	REGISTER_FIELD(animationGO, GameObject*);
	REGISTER_FIELD(enemyDetectionObject, GameObject*);
}

void BixAttackScript::Start()
{
	audioSource = owner->GetComponent<ComponentAudioSource>();
	transform = owner->GetComponent<ComponentTransform>();
	if (animationGO)
	{
		animation = animationGO->GetComponent<ComponentAnimation>();
	}

	input = App->GetModule<ModuleInput>();

	audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_OPEN);
	audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_HUM);

	playerManager = owner->GetComponent<PlayerManagerScript>();

	enemyDetection = enemyDetectionObject->GetComponent<EntityDetection>();
}

void BixAttackScript::Update(float deltaTime)
{
	// Provisional here until we have a way to delay a call to a function a certain time
	// This should go inside the PerformAttack() function but delay setting it to false by 2 seconds or smth like that
	animation->SetParameter("IsAttacking", false);

	CheckCombo();
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
	// Test Allura jumping finisher
	// TODO: Remove this once it is integrated in the combo system
	if (input->GetMouseButton(SDL_BUTTON_MIDDLE) == KeyState::DOWN && IsAttackAvailable())
	{
		LOG_VERBOSE("Allura jump finisher");
		lastAttackTime = SDL_GetTicks() / 1000.0f;
	}

	// Attack, starting the combo
	if (input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::DOWN && IsAttackAvailable())
	{
		//LOG_VERBOSE("Pressing left mouse button");

		if (animation && attackComboPhase == AttackCombo::IDLE)
		{
			attackComboPhase = AttackCombo::FIRST_ATTACK;
			animation->SetParameter("IsAttacking", true);

			PerformAttack();
		}
	}

	// Attack, continue the combo
	if (input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::REPEAT && IsAttackAvailable())
	{
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

	// If attack could be performed but no button pressed, lose the combo streak
	else if (input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::IDLE && IsAttackAvailable())
	{
		//LOG_VERBOSE("NOT pressing left mouse button");

		attackComboPhase = AttackCombo::IDLE;
		animation->SetParameter("IsAttacking", false);
		animation->SetParameter("IsAttacking_2", false);
		animation->SetParameter("IsAttacking_3", false);
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