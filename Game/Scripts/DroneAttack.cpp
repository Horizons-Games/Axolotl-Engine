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

DroneAttack::DroneAttack() : Script(), attackCooldown(0.6f), lastAttackTime(0.f), audioSource(nullptr),
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
	audioSource = static_cast<ComponentAudioSource*>(owner->GetComponent(ComponentType::AUDIOSOURCE));
	transform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));

	loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	if (animationGO)
	{
		animation = static_cast<ComponentAnimation*>(animationGO->GetComponent(ComponentType::ANIMATION));
	}
	if (bulletOriginGO)
	{
		bulletOrigin = static_cast<ComponentTransform*>(bulletOriginGO->GetComponent(ComponentType::TRANSFORM));
	}

	input = App->GetModule<ModuleInput>();
}

void DroneAttack::Update(float deltaTime)
{
	if (input->GetKey(SDL_SCANCODE_T) != KeyState::IDLE)
	{
		PerformAttack();
	}
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
		ComponentTransform* bulletTransf = static_cast<ComponentTransform*>(bullet->GetComponent(ComponentType::TRANSFORM));
		bulletTransf->SetPosition(bulletOrigin->GetGlobalPosition());
		bulletTransf->SetScale(float3(0.2f, 0.2f, 0.2f));
		bulletTransf->SetRotation(transform->GetRotation());
		bulletTransf->UpdateTransformMatrices();

		/*std::vector<ComponentScript*> gameObjectScripts =
			bullet->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT);

		for (int i = 0; i < gameObjectScripts.size(); ++i)
		{
			if (gameObjectScripts[i]->GetConstructName() == "DroneBullet")
			{
				DroneBullet* droneBulletScript = static_cast<DroneBullet*>(gameObjectScripts[i]->GetScript());
				droneBulletScript->Start();
			}
		}*/


		//Create bullet object that is going to be shooted
		//GameObject* bullet = loadedScene->Create3DGameObject("Cube", root, Premade3D::CUBE);
		
		//Set all components for the bullet manually as we still don't have prefabs
		/*ComponentTransform* bulletTransf = static_cast<ComponentTransform*>(bullet->GetComponent(ComponentType::TRANSFORM));
		bulletTransf->SetPosition(bulletOrigin->GetGlobalPosition());
		bulletTransf->SetScale(float3(0.2f, 0.2f, 0.2f));
		bulletTransf->SetRotation(transform->GetRotation());
		bulletTransf->UpdateTransformMatrices();

		bullet->CreateComponent(ComponentType::AUDIOSOURCE);*/

		ComponentScript* script = static_cast<ComponentScript*>(bullet->CreateComponent(ComponentType::SCRIPT));
		script->SetScript(App->GetScriptFactory()->ConstructScript("DroneBullet"));
		script->SetConstuctor("DroneBullet");
		script->GetScript()->SetGameObject(bullet);
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