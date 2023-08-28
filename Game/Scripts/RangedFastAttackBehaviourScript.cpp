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
	lastAttackTime(0.f), laserParticleSystem(nullptr),audioSource(nullptr),
	animation(nullptr), transform(nullptr), loadedScene(nullptr), 
	bulletVelocity(0.2f), bulletPrefab(nullptr), needReposition(false), newReposition(float3::zero)
{
	REGISTER_FIELD(attackCooldown, float);

	REGISTER_FIELD(bulletPrefab, GameObject*);
	REGISTER_FIELD(bulletVelocity, float);
	REGISTER_FIELD(laserParticleSystem, GameObject*)
}

void RangedFastAttackBehaviourScript::Start()
{
	audioSource = owner->GetComponent<ComponentAudioSource>();
	transform = owner->GetComponent<ComponentTransform>();
	animation = owner->GetComponent<ComponentAnimation>();

	loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	if (laserParticleSystem)
	{
		particleSystem = laserParticleSystem->GetComponent<ComponentParticleSystem>();
	}
}

void RangedFastAttackBehaviourScript::StartAttack()
{
	needReposition = false;
	movingToNewReposition = false;
}

void RangedFastAttackBehaviourScript::PerformAttack()
{
	if (particleSystem)
	{
		particleSystem->Play();
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

bool RangedFastAttackBehaviourScript::IsAttackAvailable() const
{
	return (SDL_GetTicks() / 1000.0f > lastAttackTime + attackCooldown);
}

bool RangedFastAttackBehaviourScript::NeedReposition() const
{
	return needReposition;
}

bool RangedFastAttackBehaviourScript::MovingToNewReposition()
{
	if (transform->GetGlobalPosition().Equals(newReposition, 0.3f))
	{
		movingToNewReposition = false;
	}
	return movingToNewReposition;
}

void RangedFastAttackBehaviourScript::ResetScriptValues()
{
	lastAttackTime = 0.0f;
	needReposition = false;
	movingToNewReposition = false;
	newReposition = float3::zero;
}