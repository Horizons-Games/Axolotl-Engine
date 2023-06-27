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

#include "GameObject/GameObject.h"

#include "Auxiliar/Audio/AudioData.h"

#include "Geometry/Ray.h"

#include "debugdraw.h"

#include <set>

REGISTERCLASS(BixAttackScript);

namespace
{
	std::vector<Ray> rays;
}

BixAttackScript::BixAttackScript() : Script(), attackCooldown(0.6f), lastAttackTime(0.f), audioSource(nullptr),
	input(nullptr), rayAttackSize(10.0f), animation(nullptr), animationGO(nullptr), transform(nullptr),
	//Provisional
	ray1GO(nullptr), ray2GO(nullptr), ray3GO(nullptr), ray4GO(nullptr),
	ray1Transform(nullptr), ray2Transform(nullptr), ray3Transform(nullptr), ray4Transform(nullptr),
	//--Provisional
	playerManager(nullptr), attackComboPhase(AttackCombo::IDLE)
{
	REGISTER_FIELD(attackCooldown, float);
	REGISTER_FIELD(rayAttackSize, float);
	REGISTER_FIELD(animationGO, GameObject*);

	//Provisional
	REGISTER_FIELD(ray1GO, GameObject*);
	REGISTER_FIELD(ray2GO, GameObject*);
	REGISTER_FIELD(ray3GO, GameObject*);
	REGISTER_FIELD(ray4GO, GameObject*);
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

	input = App->GetModule<ModuleInput>();

	audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_OPEN);
	audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_HUM);

	//Provisional
	ray1Transform = ray1GO->GetComponent<ComponentTransform>();
	ray2Transform = ray2GO->GetComponent<ComponentTransform>();
	ray3Transform = ray3GO->GetComponent<ComponentTransform>();
	ray4Transform = ray4GO->GetComponent<ComponentTransform>();

	rays.reserve(5);
	rays.push_back(Ray(transform->GetPosition(), transform->GetLocalForward()));
	rays.push_back(Ray(ray1Transform->GetGlobalPosition(), transform->GetLocalForward()));
	rays.push_back(Ray(ray2Transform->GetGlobalPosition(), transform->GetLocalForward()));
	rays.push_back(Ray(ray3Transform->GetGlobalPosition(), transform->GetLocalForward()));
	rays.push_back(Ray(ray4Transform->GetGlobalPosition(), transform->GetLocalForward()));
	//--Provisional

	playerManager = owner->GetComponent<PlayerManagerScript>();
}

void BixAttackScript::Update(float deltaTime)
{
	// Provisional here until we have a way to delay a call to a function a certain time
	// This should go inside the PerformAttack() function but delay setting it to false by 2 seconds or smth like that
	animation->SetParameter("IsAttacking", false);

	rays[0] = Ray(transform->GetPosition(), transform->GetLocalForward());
	//Provisional
	rays[1] = Ray(ray1Transform->GetGlobalPosition(), transform->GetLocalForward());
	rays[2] = Ray(ray2Transform->GetGlobalPosition(), transform->GetLocalForward());
	rays[3] = Ray(ray3Transform->GetGlobalPosition(), transform->GetLocalForward());
	rays[4] = Ray(ray4Transform->GetGlobalPosition(), transform->GetLocalForward());
	//--Provisional


#ifdef DEBUG
	for (const Ray& ray : rays)
	{
		dd::arrow(ray.pos, ray.pos + ray.dir * rayAttackSize, dd::colors::Red, 0.05f);
	}
#endif // DEBUG

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
	//Provisional
	std::set<UID> hitObjects;
	bool playSFX = false;
	for (const Ray& ray : rays)
	{
		LineSegment line(ray, rayAttackSize);
		RaycastHit hit;
		if (Physics::Raycast(line, hit, transform->GetOwner()))
		{
			playSFX = true;
			if (hit.gameObject->GetRootGO() && hit.gameObject->GetRootGO()->CompareTag("Enemy"))
			{
				if (hitObjects.insert(hit.gameObject->GetRootGO()->GetUID()).second)
				{
					// insertion could take place -> element not hit yet
					// get component health and do damage
					HealthSystem* healthScript = hit.gameObject->GetRootGO()->GetComponent<HealthSystem>();
					float damageAttack = playerManager->GetPlayerAttack();
					if (!isDeathTouched)
					{
						healthScript->TakeDamage(damageAttack);
					}
					else
					{
						healthScript->TakeDamage(healthScript->GetMaxHealth());
					}
					
				}
			}
		}
	}

	if (playSFX)
	{
		audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_CLASH);
	}
	//--Provisional
}

bool BixAttackScript::IsAttackAvailable() const
{
	return (SDL_GetTicks() / 1000.0f > lastAttackTime + attackCooldown);
}

void BixAttackScript::CheckCombo()
{
	// Attack, starting the combo
	if (input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::DOWN && IsAttackAvailable())
	{
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