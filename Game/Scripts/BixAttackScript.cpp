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

#include "HealthSystem.h"

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
input(nullptr), rayAttackSize(10.0f), animation(nullptr), animationGO(nullptr), damageAttack(20.0f),
transform(nullptr),
//Provisional
ray1GO(nullptr), ray2GO(nullptr), ray3GO(nullptr), ray4GO(nullptr),
ray1Transform(nullptr), ray2Transform(nullptr), ray3Transform(nullptr), ray4Transform(nullptr),
//--Provisional
healthScript(nullptr)
{
	REGISTER_FIELD(attackCooldown, float);
	REGISTER_FIELD(rayAttackSize, float);
	REGISTER_FIELD(damageAttack, float);
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
	audioSource = static_cast<ComponentAudioSource*>(owner->GetComponent(ComponentType::AUDIOSOURCE));
	transform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	if (animationGO)
	{
		animation = static_cast<ComponentAnimation*>(animationGO->GetComponent(ComponentType::ANIMATION));
	}

	input = App->GetModule<ModuleInput>();

	audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_OPEN);
	audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_HUM);

	//Provisional
	ray1Transform = static_cast<ComponentTransform*>(ray1GO->GetComponent(ComponentType::TRANSFORM));
	ray2Transform = static_cast<ComponentTransform*>(ray2GO->GetComponent(ComponentType::TRANSFORM));
	ray3Transform = static_cast<ComponentTransform*>(ray3GO->GetComponent(ComponentType::TRANSFORM));
	ray4Transform = static_cast<ComponentTransform*>(ray4GO->GetComponent(ComponentType::TRANSFORM));

	rays.reserve(5);
	rays.push_back(Ray(transform->GetPosition(), transform->GetLocalForward()));
	rays.push_back(Ray(ray1Transform->GetGlobalPosition(), transform->GetLocalForward()));
	rays.push_back(Ray(ray2Transform->GetGlobalPosition(), transform->GetLocalForward()));
	rays.push_back(Ray(ray3Transform->GetGlobalPosition(), transform->GetLocalForward()));
	rays.push_back(Ray(ray4Transform->GetGlobalPosition(), transform->GetLocalForward()));
	//--Provisional

	std::vector<ComponentScript*> gameObjectScripts =
		owner->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT);

	for (int i = 0; i < gameObjectScripts.size(); ++i)
	{
		if (gameObjectScripts[i]->GetConstructName() == "HealthSystem")
		{
			healthScript = static_cast<HealthSystem*>(gameObjectScripts[i]->GetScript());
		}
	}
}

void BixAttackScript::Update(float deltaTime)
{
	if (healthScript && !healthScript->EntityIsAlive())
	{
		return;
	}

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

	// Attack
	if (input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::DOWN)
	{
		PerformAttack();
	}
}

void BixAttackScript::PerformAttack()
{
	if (isAttackAvailable())
	{
		if (animation)
		{
			animation->SetParameter("IsAttacking", true);
		}

		lastAttackTime = SDL_GetTicks() / 1000.0f;

		audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_SWING);

		CheckCollision();
	}
}

void BixAttackScript::CheckCollision()
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
			if (hit.gameObject->GetRootGO())
			{
				if (hit.gameObject->GetRootGO()->CompareTag("Enemy"))
				{
					if (hitObjects.insert(hit.gameObject->GetRootGO()->GetUID()).second)
					{
						// insertion could take place -> element not hit yet
						//get component health and do damage
						std::vector<ComponentScript*> gameObjectScripts =
							hit.gameObject->GetRootGO()->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT);

						for (int i = 0; i < gameObjectScripts.size(); ++i)
						{
							if (gameObjectScripts[i]->GetConstructName() == "HealthSystem")
							{
								HealthSystem* healthScript = static_cast<HealthSystem*>(gameObjectScripts[i]->GetScript());
								healthScript->TakeDamage(damageAttack);
							}
						}
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

bool BixAttackScript::isAttackAvailable()
{
	return (SDL_GetTicks() / 1000.0f > lastAttackTime + attackCooldown);
}
