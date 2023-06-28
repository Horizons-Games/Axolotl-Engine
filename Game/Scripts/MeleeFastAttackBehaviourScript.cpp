#include "MeleeFastAttackBehaviourScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"

#include "../Scripts/HealthSystem.h"

#include "Physics/Physics.h"

#include "../MathGeoLib/Include/Geometry/Ray.h"

#include "debugdraw.h"
#include <set>

REGISTERCLASS(MeleeFastAttackBehaviourScript);

MeleeFastAttackBehaviourScript::MeleeFastAttackBehaviourScript() : Script(), attackCooldown(6.f), lastAttackTime(0.f),
	damageAttack(10.f), ownerTransform(nullptr), rayAttackSize(10.f),
	//Provisional
	ray1GO(nullptr), ray2GO(nullptr), ray3GO(nullptr), ray4GO(nullptr),
	ray1Transform(nullptr), ray2Transform(nullptr), ray3Transform(nullptr), ray4Transform(nullptr)
	//--Provisional
{
	REGISTER_FIELD(attackCooldown, float);
	REGISTER_FIELD(damageAttack, float);

	//Provisional
	REGISTER_FIELD(ray1GO, GameObject*);
	REGISTER_FIELD(ray2GO, GameObject*);
	REGISTER_FIELD(ray3GO, GameObject*);
	REGISTER_FIELD(ray4GO, GameObject*);

	REGISTER_FIELD(rayAttackSize, float);
	//--Provisional
}

void MeleeFastAttackBehaviourScript::Start()
{
	ownerTransform = owner->GetComponent<ComponentTransform>();

	//Provisional
	ray1Transform = ray1GO->GetComponent<ComponentTransform>();
	ray2Transform = ray2GO->GetComponent<ComponentTransform>();
	ray3Transform = ray3GO->GetComponent<ComponentTransform>();
	ray4Transform = ray4GO->GetComponent<ComponentTransform>();

	rays.reserve(5);
	rays.push_back(Ray(ownerTransform->GetPosition(), ownerTransform->GetLocalForward()));
	rays.push_back(Ray(ray1Transform->GetGlobalPosition(), ownerTransform->GetLocalForward()));
	rays.push_back(Ray(ray2Transform->GetGlobalPosition(), ownerTransform->GetLocalForward()));
	rays.push_back(Ray(ray3Transform->GetGlobalPosition(), ownerTransform->GetLocalForward()));
	rays.push_back(Ray(ray4Transform->GetGlobalPosition(), ownerTransform->GetLocalForward()));
	//--Provisional
}

void MeleeFastAttackBehaviourScript::Update(float deltaTime)
{
	rays[0] = Ray(ownerTransform->GetPosition(), ownerTransform->GetLocalForward());
	//Provisional
	rays[1] = Ray(ray1Transform->GetGlobalPosition(), ownerTransform->GetLocalForward());
	rays[2] = Ray(ray2Transform->GetGlobalPosition(), ownerTransform->GetLocalForward());
	rays[3] = Ray(ray3Transform->GetGlobalPosition(), ownerTransform->GetLocalForward());
	rays[4] = Ray(ray4Transform->GetGlobalPosition(), ownerTransform->GetLocalForward());
	//--Provisional


#ifdef DEBUG
	for (const Ray& ray : rays)
	{
		dd::arrow(ray.pos, ray.pos + ray.dir * rayAttackSize, dd::colors::Red, 0.05f);
	}
#endif // DEBUG
}

void MeleeFastAttackBehaviourScript::PerformAttack()
{
	lastAttackTime = SDL_GetTicks() / 1000.0f;
	CheckCollision();
}

void MeleeFastAttackBehaviourScript::CheckCollision()
{
	std::set<UID> hitObjects;
	bool playSFX = false;
	for (const Ray& ray : rays)
	{
		LineSegment line(ray, rayAttackSize);
		RaycastHit hit;
		if (Physics::Raycast(line, hit, ownerTransform->GetOwner()))
		{
			playSFX = true;
			if (hit.gameObject->GetRootGO() && hit.gameObject->GetRootGO()->CompareTag("Player"))
			{
				if (hitObjects.insert(hit.gameObject->GetRootGO()->GetUID()).second)
				{
					// insertion could take place -> element not hit yet
					// get component health and do damage
					HealthSystem* healthScript = hit.gameObject->GetRootGO()->GetComponent<HealthSystem>();
					healthScript->TakeDamage(damageAttack);
				}
			}
		}
	}
}

bool MeleeFastAttackBehaviourScript::IsAttackAvailable()
{
	return (SDL_GetTicks() / 1000.0f > lastAttackTime + attackCooldown);
}