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

RangedFastAttackBehaviourScript::RangedFastAttackBehaviourScript() : Script(), attackCooldown(5.f), lastAttackTime(0.f), LaserParticleSystem(nullptr),
	audioSource(nullptr),
	animation(nullptr), transform(nullptr), bulletOriginGO(nullptr), bulletOrigin(nullptr), loadedScene(nullptr), 
	bulletVelocity(0.2f), bulletPrefab(nullptr), needReposition(false), newReposition(0,0,0)
{
	REGISTER_FIELD(attackCooldown, float);

	REGISTER_FIELD(bulletOriginGO, GameObject*);
	REGISTER_FIELD(bulletPrefab, GameObject*);
	REGISTER_FIELD(bulletVelocity, float);
	REGISTER_FIELD(LaserParticleSystem, GameObject*)
}

void RangedFastAttackBehaviourScript::Start()
{
	audioSource = owner->GetComponent<ComponentAudioSource>();
	transform = owner->GetComponent<ComponentTransform>();
	animation = owner->GetComponent<ComponentAnimation>();

	loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	if (bulletOriginGO)
	{
		bulletOrigin = bulletOriginGO->GetComponent<ComponentTransform>();
	}

	if (LaserParticleSystem)
	{
		particleSystem = LaserParticleSystem->GetComponent<ComponentParticleSystem>();
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
	GameObject* root = loadedScene->GetRoot();
	GameObject* bullet = loadedScene->DuplicateGameObject(bulletPrefab->GetName(), bulletPrefab, root);
	ComponentTransform* bulletTransf = bullet->GetComponent<ComponentTransform>();

	// Set the new bullet in the drone, ready for being shooted
	bulletTransf->SetLocalPosition(bulletOrigin->GetGlobalPosition());
	bulletTransf->SetLocalScale(float3(0.2f, 0.2f, 0.2f));
	bulletTransf->SetLocalRotation(transform->GetGlobalRotation());
	bulletTransf->UpdateTransformMatrices();

	// Attack the DroneFastBullet script to the new bullet to give it its logic
	ComponentScript* script = bullet->CreateComponent<ComponentScript>();
	script->SetScript(App->GetScriptFactory()->ConstructScript("RangedFastAttackBullet"));
	script->SetConstuctor("RangedFastAttackBullet");
	script->GetScript()->SetGameObject(bullet);
	script->GetScript()->SetApplication(App);

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
