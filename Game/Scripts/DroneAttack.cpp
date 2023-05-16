#include "DroneAttack.h"

#include "Application.h"

#include "ModuleInput.h"

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
							bulletOriginGO(nullptr), bulletOrigin(nullptr), loadedScene(nullptr)
{
	REGISTER_FIELD(attackCooldown, float);
	REGISTER_FIELD(animationGO, GameObject*);
	REGISTER_FIELD(bulletOriginGO, GameObject*);
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
}

void DroneAttack::PerformAttack()
{
	if (isAttackAvailable())
	{
		if (animation)
			animation->SetParameter("attack", true);

		
		GameObject* root = App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot();
		
		GameObject* bullet = loadedScene->Create3DGameObject("Cube", root, Premade3D::CUBE);
		
		//bullet->position = bulletOrigin->position;

		lastAttackTime = SDL_GetTicks() / 1000.0f;

		audioSource->PostEvent(audio::SFX_DRON_SHOT_01);
	}
}

bool DroneAttack::isAttackAvailable()
{
	return (SDL_GetTicks() / 1000.0f > lastAttackTime + attackCooldown);
}
