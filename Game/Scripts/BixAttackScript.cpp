#include "BixAttackScript.h"

#include "ModuleInput.h"

#include "Scene/Scene.h"

#include "Components/ComponentAudioSource.h"
#include "Components/ComponentMeshCollider.h"

#include "GameObject/GameObject.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(BixAttackScript);

BixAttackScript::BixAttackScript() : Script(), attackCooldown(0.6f), lastAttackTime(0.f), audioSource(nullptr),
									attackColliderGO(nullptr), input(nullptr), attackCollider(nullptr)
{
	REGISTER_FIELD(attackCooldown, float);
	REGISTER_FIELD(attackColliderGO, GameObject*);
}

void BixAttackScript::Start()
{
	audioSource = static_cast<ComponentAudioSource*>(owner->GetComponent(ComponentType::AUDIOSOURCE));
	attackCollider = static_cast<ComponentMeshCollider*>(attackColliderGO->GetComponent(ComponentType::MESHCOLLIDER));

	input = App->GetModule<ModuleInput>();

	audioSource->PostEvent(audio::SFX_PLAYER_LIGHTSABER_OPEN);
	audioSource->PostEvent(audio::SFX_PLAYER_LIGHTSABER_HUM);
}

void BixAttackScript::Update(float deltaTime)
{
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

		//if (attackCollider->IsColliding())
		

	}
}

bool BixAttackScript::isAttackAvailable()
{
	return (SDL_GetTicks() / 1000.0f > lastAttackTime + attackCooldown);
}
