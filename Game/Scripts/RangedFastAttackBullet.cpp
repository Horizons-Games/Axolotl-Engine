#include "RangedFastAttackBullet.h"

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
#include "Components/ComponentRigidBody.h"

#include "../Scripts/HealthSystem.h"

#include "Auxiliar/Audio/AudioData.h"
#include <AxoLog.h>

REGISTERCLASS(RangedFastAttackBullet);

RangedFastAttackBullet::RangedFastAttackBullet() : Script(), parentTransform(nullptr), rigidBody(nullptr), velocity(20.0f), audioSource(nullptr),
	bulletLifeTime(10.0f), damageAttack(10.0f), rayAttackSize(100.0f), originTime(0.0f)
{
}

void RangedFastAttackBullet::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	parentTransform = owner->GetParent()->GetComponent<ComponentTransform>();
	audioSource = owner->GetComponent<ComponentAudioSource>();

	//rigidBody->SetDefaultPosition();
	rigidBody->SetDrawCollider(true);

	btRigidBody* btRb = rigidBody->GetRigidBody();
	btRb->setLinearVelocity(
		btVector3(
			parentTransform->GetGlobalForward().x,
			0,
			parentTransform->GetGlobalForward().z) * velocity);

	originTime = SDL_GetTicks() / 1000.0f;
}

void RangedFastAttackBullet::Update(float deltaTime)
{
#ifdef DEBUG
	Ray rayDebug(owner->GetComponent<ComponentTransform>()->GetPosition(), owner->GetComponent<ComponentTransform>()->GetLocalForward());
	dd::arrow(rayDebug.pos, rayDebug.pos + rayDebug.dir * rayAttackSize, dd::colors::Red, 0.05f);
#endif // DEBUG

	if (SDL_GetTicks() / 1000.0f > originTime + bulletLifeTime)
	{
		DestroyBullet();
	}
}

void RangedFastAttackBullet::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->IsTrigger())
	{
		return;
	}

	if (other->GetOwner()->CompareTag("Player"))
	{
		HealthSystem* playerHealthScript = other->GetOwner()->GetComponent<HealthSystem>();
		playerHealthScript->TakeDamage(damageAttack);
	}

	audioSource->PostEvent(AUDIO::SFX::NPC::DRON::SHOT_IMPACT_01); //Provisional sfx
	DestroyBullet();
}

void RangedFastAttackBullet::DestroyBullet()
{
	App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
}
