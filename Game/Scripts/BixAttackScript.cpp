#include "BixAttackScript.h"

#include "Application.h"

#include "ModuleInput.h"

#include "Physics/Physics.h"

#include "Components/ComponentAudioSource.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"

#include "GameObject/GameObject.h"

#include "Auxiliar/Audio/AudioData.h"

#include "Geometry/Ray.h"

#include "debugdraw.h"

REGISTERCLASS(BixAttackScript);

BixAttackScript::BixAttackScript() : Script(), attackCooldown(0.6f), lastAttackTime(0.f), audioSource(nullptr),
									input(nullptr), rayAttackSize(3.0f),
									transform(nullptr)
{
	REGISTER_FIELD(attackCooldown, float);
	REGISTER_FIELD(rayAttackSize, float);
}

void BixAttackScript::Start()
{
	audioSource = static_cast<ComponentAudioSource*>(owner->GetComponent(ComponentType::AUDIOSOURCE));
	transform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));

	input = App->GetModule<ModuleInput>();

	audioSource->PostEvent(audio::SFX_PLAYER_LIGHTSABER_OPEN);
	audioSource->PostEvent(audio::SFX_PLAYER_LIGHTSABER_HUM);
}

void BixAttackScript::Update(float deltaTime)
{
	//DEBUG:
	Ray ray(transform->GetPosition(), transform->GetLocalForward());
	dd::arrow(ray.pos, ray.pos + ray.dir * rayAttackSize, dd::colors::Red, 0.05f);

	// Attack
	if (input->GetKey(SDL_SCANCODE_Q) != KeyState::IDLE)
	{
		PerformAttack();
	}
}

void BixAttackScript::PerformAttack()
{
	if (isAttackAvailable())
	{
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

bool BixAttackScript::isAttackAvailable()
{
	return (SDL_GetTicks() / 1000.0f > lastAttackTime + attackCooldown);
}
