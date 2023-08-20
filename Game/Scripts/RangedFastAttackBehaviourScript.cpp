#include "RangedFastAttackBehaviourScript.h"

#include "Application.h"

#include "Geometry/Ray.h"
#include "debugdraw.h"

#include "Scripting/ScriptFactory.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

#include "Components/ComponentAudioSource.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentParticleSystem.h"

#include "../Scripts/RangedFastAttackBullet.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(RangedFastAttackBehaviourScript);

RangedFastAttackBehaviourScript::RangedFastAttackBehaviourScript() : Script(), attackCooldown(5.f), 
	lastAttackTime(0.f), particleSystemShot(nullptr), particleSystemPreShot(nullptr), audioSource(nullptr), shootPosition(nullptr),
	particleTransform(nullptr), animation(nullptr), transform(nullptr), loadedScene(nullptr), preShotDuration(0.0f),
	bulletVelocity(0.2f), bulletPrefab(nullptr), needReposition(false), newReposition(0,0,0), isPreShooting(false),
	preShootingTime(0.0f), particlePreShotTransform(nullptr)
{
	REGISTER_FIELD(attackCooldown, float);

	REGISTER_FIELD(bulletPrefab, GameObject*);
	REGISTER_FIELD(bulletVelocity, float);
	REGISTER_FIELD(shootPosition, ComponentTransform*);
	REGISTER_FIELD(particleSystemShot, ComponentParticleSystem*);
	REGISTER_FIELD(particleSystemPreShot, ComponentParticleSystem*);
	REGISTER_FIELD(preShotDuration, float);
}

void RangedFastAttackBehaviourScript::Start()
{
	audioSource = owner->GetComponent<ComponentAudioSource>();
	transform = owner->GetComponent<ComponentTransform>();
	if (particleSystemShot)
	{
		particleTransform = particleSystemShot->GetOwner()->GetComponent<ComponentTransform>();
	}
	if (particleSystemPreShot)
	{
		particlePreShotTransform = particleSystemPreShot->GetOwner()->GetComponent<ComponentTransform>();
	}
	animation = owner->GetComponent<ComponentAnimation>();

	loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	isPreShooting = false;
}

void RangedFastAttackBehaviourScript::Update(float deltaTime)
{
	if (!isPreShooting)
	{
		return;
	}


	preShootingTime += deltaTime;
	if (shootPosition)
	{
		particlePreShotTransform->SetGlobalPosition(shootPosition->GetGlobalPosition());
	}

	if (preShootingTime >= preShotDuration)
	{
		ShootBullet();
		isPreShooting = false;
		preShootingTime = 0.0f;
	}
}

void RangedFastAttackBehaviourScript::StartAttack()
{
	needReposition = false;
	movingToNewReposition = false;
}

void RangedFastAttackBehaviourScript::PerformAttack()
{
	isPreShooting = true;
	if (particleSystemPreShot)
	{
		particleSystemPreShot->Play();
	}
}

void RangedFastAttackBehaviourScript::ShootBullet()
{
	if (particleSystemShot)
	{
		if (shootPosition)
		{
			particleTransform->SetGlobalPosition(shootPosition->GetGlobalPosition());
		}
		particleSystemShot->Play();
	}

	animation->SetParameter("IsAttacking", true);

	// Create a new bullet
	GameObject* bullet = loadedScene->DuplicateGameObject(bulletPrefab->GetName(), bulletPrefab, owner);

	// Attach the RangedFastAttackBullet script to the new bullet to give it its logic
	ComponentScript* script = bullet->CreateComponent<ComponentScript>();
	script->SetScript(App->GetScriptFactory()->ConstructScript("RangedFastAttackBullet"));
	script->SetConstuctor("RangedFastAttackBullet");
	script->GetScript()->SetOwner(bullet);

	bullet->GetComponent<RangedFastAttackBullet>()->SetBulletVelocity(bulletVelocity);

	// Once the engine automatically runs the Start() for newly created objects, delete this line
	script->Start();

	lastAttackTime = SDL_GetTicks() / 1000.0f;
	audioSource->PostEvent(AUDIO::SFX::NPC::DRON::SHOT_01);

	//Set reposition
	needReposition = true;
}

void RangedFastAttackBehaviourScript::Reposition(float3 nextPosition)
{
	needReposition = false;
	movingToNewReposition = true;
	//set new target position
	owner->GetComponent<ComponentRigidBody>()->SetPositionTarget(nextPosition);
	owner->GetComponent<ComponentRigidBody>()->SetKpForce(1.0f);
}

void RangedFastAttackBehaviourScript::InterruptAttack()
{
	isPreShooting = false;
	preShootingTime = 0.0f;
	lastAttackTime = SDL_GetTicks() / 1000.0f - attackCooldown / 2.0f;
	if (particleSystemPreShot)
	{
		particleSystemPreShot->Stop();
	}
}

bool RangedFastAttackBehaviourScript::IsAttackAvailable() const
{
	return (SDL_GetTicks() / 1000.0f > lastAttackTime + attackCooldown && !isPreShooting);
}

bool RangedFastAttackBehaviourScript::NeedReposition() const
{
	return needReposition;
}

bool RangedFastAttackBehaviourScript::IsPreShooting() const
{
	return isPreShooting;
}

bool RangedFastAttackBehaviourScript::MovingToNewReposition()
{
	if (transform->GetGlobalPosition().Equals(newReposition, 0.3f))
	{
		movingToNewReposition = false;
	}
	return movingToNewReposition;
}
