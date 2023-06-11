#include "DroneAttack.h"

#include "Application.h"

#include "ModuleInput.h"
#include "Scripting/ScriptFactory.h"

#include "Scene/Scene.h"
#include "ModuleScene.h"

#include "Components/ComponentAudioSource.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentScript.h"

#include "DroneBullet.h"

#include "GameObject/GameObject.h"

#include "Auxiliar/Audio/AudioData.h"

#include "Geometry/Ray.h"

#include "debugdraw.h"

REGISTERCLASS(DroneAttack);

DroneAttack::DroneAttack() : Script(), attackCooldown(5.f), lastAttackTime(0.f), audioSource(nullptr),
input(nullptr), animation(nullptr), animationGO(nullptr), transform(nullptr),
bulletOriginGO(nullptr), bulletOrigin(nullptr), loadedScene(nullptr), bulletVelocity(0.2f),
bulletPrefab(nullptr)
{
	REGISTER_FIELD(attackCooldown, float);
	REGISTER_FIELD(animationGO, GameObject*);
	REGISTER_FIELD(bulletOriginGO, GameObject*);
	REGISTER_FIELD(bulletPrefab, GameObject*);
	REGISTER_FIELD(bulletVelocity, float);
}

void DroneAttack::Start()
{
	audioSource = owner->GetComponent<ComponentAudioSource>();
	transform = owner->GetComponent<ComponentTransform>();

	loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	if (animationGO)
	{
		animation = animationGO->GetComponent<ComponentAnimation>();
	}
	if (bulletOriginGO)
	{
		bulletOrigin = bulletOriginGO->GetComponent<ComponentTransform>();
	}

	input = App->GetModule<ModuleInput>();
}

void DroneAttack::Update(float deltaTime)
{
}

void DroneAttack::PerformAttack()
{
	if (isAttackAvailable())
	{
		if (animation)
		{
			animation->SetParameter("attack", true);
		}

		GameObject* root = loadedScene->GetRoot();

		GameObject* bullet = loadedScene->DuplicateGameObject(
			bulletPrefab->GetName(), bulletPrefab, root);
		ComponentTransform* bulletTransf = bullet->GetComponent<ComponentTransform>();
		bulletTransf->SetPosition(bulletOrigin->GetGlobalPosition());
		bulletTransf->SetScale(float3(0.2f, 0.2f, 0.2f));
		bulletTransf->SetRotation(transform->GetGlobalRotation());
		bulletTransf->UpdateTransformMatrices();

		ComponentScript* script = bullet->CreateComponent<ComponentScript>();
		script->SetScript(App->GetScriptFactory()->ConstructScript("DroneBullet"));
		script->SetConstuctor("DroneBullet");
		script->GetScript()->SetOwner(bullet);
		script->GetScript()->SetApplication(App);
		script->Start();//Should be done automatically but the engine still does not do the Start for runtime created scripts

		lastAttackTime = SDL_GetTicks() / 1000.0f;

		audioSource->PostEvent(AUDIO::SFX::NPC::DRON::SHOT_01);
	}
}

bool DroneAttack::isAttackAvailable()
{
	return (SDL_GetTicks() / 1000.0f > lastAttackTime + attackCooldown);
}