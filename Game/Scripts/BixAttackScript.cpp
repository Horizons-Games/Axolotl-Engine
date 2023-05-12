#include "BixAttackScript.h"

#include "ModuleInput.h"

#include "Scene/Scene.h"

#include "Components/ComponentAudioSource.h"

#include "GameObject/GameObject.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(BixAttackScript);

BixAttackScript::BixAttackScript() : Script(), componentAudio(nullptr)
{
}

void BixAttackScript::Start()
{
	componentAudio = static_cast<ComponentAudioSource*>(owner->GetComponent(ComponentType::AUDIOSOURCE));

	input = App->GetModule<ModuleInput>();
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
	componentAudio->PostEvent(audio::SFX_PLAYER_LIGHTSABER_SWING);
}
