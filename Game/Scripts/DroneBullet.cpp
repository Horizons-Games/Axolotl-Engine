#include "DroneBullet.h"

#include "Application.h"

#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "Scripting/ScriptFactory.h"

#include "Physics/Physics.h"
#include "Geometry/Ray.h"
#include "debugdraw.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentScript.h"

#include "../Scripts/HealthSystem.h"

#include "Auxiliar/Audio/AudioData.h"



REGISTERCLASS(DroneBullet);

DroneBullet::DroneBullet() : Script(), transform(nullptr), velocity(0.2f), audioSource(nullptr), bulletLifeTime(10.0f),
	damageAttack(10.0f), rayAttackSize(100.0f), originTime(0.0f)
{
}

void DroneBullet::Start()
{
	transform = owner->GetComponent<ComponentTransform>();
	audioSource = owner->GetComponent<ComponentAudioSource>();

	originTime = SDL_GetTicks() / 1000.0f;
}

void DroneBullet::Update(float deltaTime)
{
#ifdef DEBUG
	Ray rayDebug(transform->GetPosition(), transform->GetLocalForward());
	dd::arrow(rayDebug.pos, rayDebug.pos + rayDebug.dir * rayAttackSize, dd::colors::Red, 0.05f);
#endif // DEBUG

	ShootBullet(deltaTime);

	CheckCollision();

	if (SDL_GetTicks() / 1000.0f > originTime + bulletLifeTime)
	{
		DestroyBullet();
	}
}

void DroneBullet::ShootBullet(float deltaTime)
{
	transform->SetPosition(transform->GetGlobalPosition() + transform->GetGlobalForward() * velocity * deltaTime * 1000);
	transform->UpdateTransformMatrices();
}

void DroneBullet::CheckCollision()
{
	Ray ray(transform->GetGlobalPosition(), transform->GetGlobalForward());
	LineSegment line(ray, rayAttackSize);
	RaycastHit hit;

	if (Physics::Raycast(line, hit, transform->GetOwner()))
	{
		// We should avoid using GetRootGO()
		if (hit.gameObject->GetRootGO() && hit.gameObject->GetRootGO()->CompareTag("Player"))
		{
			HealthSystem* playerHealthScript = hit.gameObject->GetRootGO()->GetComponent<HealthSystem>();
			playerHealthScript->TakeDamage(damageAttack);
		}

		audioSource->PostEvent(AUDIO::SFX::NPC::DRON::SHOT_IMPACT_01); //Provisional sfx
		DestroyBullet();
	}
}

void DroneBullet::DestroyBullet()
{
	App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
}
