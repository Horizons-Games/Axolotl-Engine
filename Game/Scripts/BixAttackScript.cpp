#include "BixAttackScript.h"

#include "ModuleInput.h"

#include "Scene/Scene.h"

#include "Components/ComponentAudioSource.h"

#include "GameObject/GameObject.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(BixAttackScript);

BixAttackScript::BixAttackScript() : Script(), attackCooldown(0.6f), lastAttackTime(0.f), componentAudio(nullptr), input(nullptr)
{
	REGISTER_FIELD(attackCooldown, float);
}

void BixAttackScript::Start()
{
	componentAudio = static_cast<ComponentAudioSource*>(owner->GetComponent(ComponentType::AUDIOSOURCE));

	input = App->GetModule<ModuleInput>();

	componentAudio->PostEvent(audio::SFX_PLAYER_LIGHTSABER_OPEN);
	componentAudio->PostEvent(audio::SFX_PLAYER_LIGHTSABER_HUM);
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
		componentAudio->PostEvent(audio::SFX_PLAYER_LIGHTSABER_SWING);
	}
}

bool BixAttackScript::isAttackAvailable()
{
	return (SDL_GetTicks() / 1000.0f > lastAttackTime + attackCooldown);
}
