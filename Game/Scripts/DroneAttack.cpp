#include "DroneAttack.h"

#include "Application.h"

#include "ModuleInput.h"

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
input(nullptr), rayAttackSize(3.0f), animation(nullptr), animationGO(nullptr),
transform(nullptr)
{
	REGISTER_FIELD(attackCooldown, float);
	REGISTER_FIELD(rayAttackSize, float);
	REGISTER_FIELD(animationGO, GameObject*);
}

void DroneAttack::Start()
{
	audioSource = static_cast<ComponentAudioSource*>(owner->GetComponent(ComponentType::AUDIOSOURCE));
	transform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	if (animationGO)
		animation = static_cast<ComponentAnimation*>(animationGO->GetComponent(ComponentType::ANIMATION));

	input = App->GetModule<ModuleInput>();

	audioSource->PostEvent(audio::SFX_PLAYER_LIGHTSABER_OPEN);
	audioSource->PostEvent(audio::SFX_PLAYER_LIGHTSABER_HUM);
}

void DroneAttack::Update(float deltaTime)
{
	Ray ray(transform->GetPosition(), transform->GetLocalForward());
	dd::arrow(ray.pos, ray.pos + ray.dir * rayAttackSize, dd::colors::Red, 0.05f);

	// Attack
	if (input->GetKey(SDL_SCANCODE_Q) != KeyState::IDLE)
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

		lastAttackTime = SDL_GetTicks() / 1000.0f;

		audioSource->PostEvent(audio::SFX_PLAYER_LIGHTSABER_SWING);

		Ray ray(transform->GetPosition(), transform->GetLocalForward());
		LineSegment line(ray, rayAttackSize);
		if (Physics::RaycastFirst(line, owner))
		{
			audioSource->PostEvent(audio::SFX_PLAYER_LIGHTSABER_CLASH);
		}
	}
}

bool DroneAttack::isAttackAvailable()
{
	return (SDL_GetTicks() / 1000.0f > lastAttackTime + attackCooldown);
}
