#include "DroneAttack.h"

#include "Application.h"

#include "ModuleInput.h"
#include "Scripting/ScriptFactory.h"

#include "Scene/Scene.h"
#include "ModuleScene.h"

#include "Physics/Physics.h"

#include "Components/ComponentAudioSource.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAnimation.h"

#include "GameObject/GameObject.h"

#include "Auxiliar/Audio/AudioData.h"

#include "Geometry/Ray.h"

#include "debugdraw.h"

REGISTERCLASS(DroneAttack);

DroneAttack::DroneAttack() : Script(), attackCooldown(0.6f), lastAttackTime(0.f), audioSource(nullptr),
							input(nullptr), animation(nullptr), animationGO(nullptr), transform(nullptr),
							bulletOriginGO(nullptr), bulletOrigin(nullptr), loadedScene(nullptr), bulletVelocity(0.1f)
{
	REGISTER_FIELD(attackCooldown, float);
	REGISTER_FIELD(animationGO, GameObject*);
	REGISTER_FIELD(bulletOriginGO, GameObject*);
	REGISTER_FIELD(bulletVelocity, float);
}

void DroneAttack::Start()
{
	audioSource = static_cast<ComponentAudioSource*>(owner->GetComponent(ComponentType::AUDIOSOURCE));
	transform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));

	loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	if (animationGO)
		animation = static_cast<ComponentAnimation*>(animationGO->GetComponent(ComponentType::ANIMATION));
	if (bulletOriginGO)
		bulletOrigin = static_cast<ComponentTransform*>(bulletOriginGO->GetComponent(ComponentType::TRANSFORM));

	input = App->GetModule<ModuleInput>();
}

void DroneAttack::Update(float deltaTime)
{
	// Attack
	if (input->GetKey(SDL_SCANCODE_T) != KeyState::IDLE)
	{
		PerformAttack();
	}

	//Provisional
	ShootBullets(deltaTime);
}

void DroneAttack::PerformAttack()
{
	if (isAttackAvailable())
	{
		if (animation)
			animation->SetParameter("attack", true);

		
		GameObject* root = App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot();

		GameObject* bullet = loadedScene->Create3DGameObject("Cube", root, Premade3D::CUBE);
		
		ComponentTransform* bulletTransf = static_cast<ComponentTransform*>(bullet->GetComponent(ComponentType::TRANSFORM));
		

		bulletTransf->SetPosition(bulletOrigin->GetGlobalPosition());
		bulletTransf->SetScale(float3(0.2f, 0.2f, 0.2f));
		bulletTransf->UpdateTransformMatrices();
		bullets.push_back(bulletTransf);
		/*bullet->CreateComponent(ComponentType::SCRIPT);
		ComponentScript* script = static_cast<ComponentScript*>(bullet->GetComponent(ComponentType::SCRIPT));
		script->SetScript(App->GetScriptFactory()->GetScript("DroneBullet"));*/

		lastAttackTime = SDL_GetTicks() / 1000.0f;

		audioSource->PostEvent(audio::SFX_DRON_SHOT_01);
	}
}

bool DroneAttack::isAttackAvailable()
{
	return (SDL_GetTicks() / 1000.0f > lastAttackTime + attackCooldown);
}


void DroneAttack::ShootBullets(float deltaTime) {
	for (int i = 0; i < bullets.size(); i++) {
		bullets[i]->SetPosition(bullets[i]->GetGlobalPosition() + bullets[i]->GetLocalForward() * bulletVelocity * deltaTime * 1000);
		bullets[i]->UpdateTransformMatrices();
		Ray ray(bullets[i]->GetPosition(), bullets[i]->GetLocalForward());
		LineSegment line(ray, 3.0f);
		if (Physics::RaycastFirst(line, bullets[i]->GetOwner()))
		{
			/*ModuleScene* moduleScene = App->GetModule<ModuleScene>();
			Scene* loadedScene = moduleScene->GetLoadedScene(); 
			GameObject* auxBullet = bullets[i]->GetOwner();
			bullets.erase(bullets.begin() + i);
			loadedScene->DestroyGameObject(auxBullet);*/

			//WARRADA
			bullets[i]->GetOwner()->Disable();
			bullets.erase(bullets.begin() + i);

			audioSource->PostEvent(audio::SFX_PLAYER_LIGHTSABER_CLASH);
		}

	}
}