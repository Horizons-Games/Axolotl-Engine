#include "DroneAttack.h"

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

#include "../Scripts/DroneBullet.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(DroneAttack);

DroneAttack::DroneAttack() : Script(), attackCooldown(5.f), lastAttackTime(0.f), audioSource(nullptr),
	animation(nullptr), transform(nullptr), bulletOriginGO(nullptr), bulletOrigin(nullptr), loadedScene(nullptr), 
	bulletVelocity(0.2f), bulletPrefab(nullptr)
{
	REGISTER_FIELD(attackCooldown, float);

	REGISTER_FIELD(bulletOriginGO, GameObject*);
	REGISTER_FIELD(bulletPrefab, GameObject*);
	REGISTER_FIELD(bulletVelocity, float);
}

void DroneAttack::Start()
{
	audioSource = owner->GetComponent<ComponentAudioSource>();
	transform = owner->GetComponent<ComponentTransform>();
	animation = owner->GetComponent<ComponentAnimation>();

	loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	if (bulletOriginGO)
	{
		bulletOrigin = bulletOriginGO->GetComponent<ComponentTransform>();
	}
}

void DroneAttack::PerformAttack()
{
	if (IsAttackAvailable())
	{
		animation->SetParameter("attack", true);

		// Create a new bullet
		GameObject* root = loadedScene->GetRoot();
		GameObject* bullet = loadedScene->DuplicateGameObject(bulletPrefab->GetName(), bulletPrefab, root);
		ComponentTransform* bulletTransf = bullet->GetComponent<ComponentTransform>();

		// Set the new bullet in the drone, ready for being shooted
		bulletTransf->SetPosition(bulletOrigin->GetGlobalPosition());
		bulletTransf->SetScale(float3(0.2f, 0.2f, 0.2f));
		bulletTransf->SetRotation(transform->GetGlobalRotation());
		bulletTransf->UpdateTransformMatrices();

		// Attack the DroneBullet script to the new bullet to give it its logic
		ComponentScript* script = bullet->CreateComponent<ComponentScript>();
		script->SetScript(App->GetScriptFactory()->ConstructScript("DroneBullet"));
		script->SetConstuctor("DroneBullet");
		script->GetScript()->SetGameObject(bullet);
		script->GetScript()->SetApplication(App);

		// Once the engine automatically runs the Start() for newly created objects, delete this line
		script->Start();

		lastAttackTime = SDL_GetTicks() / 1000.0f;
		audioSource->PostEvent(AUDIO::SFX::NPC::DRON::SHOT_01);
	}
}

bool DroneAttack::IsAttackAvailable()
{
	return (SDL_GetTicks() / 1000.0f > lastAttackTime + attackCooldown);
}