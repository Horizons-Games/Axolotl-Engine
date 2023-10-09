#include "TriggerSewersMusicExit.h"

#include "Components/ComponentAudioSource.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentPlayer.h"
#include "../Scripts/TriggerSewersMusic.h"

#include "GameObject/GameObject.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(TriggerSewersMusicExit);

TriggerSewersMusicExit::TriggerSewersMusicExit() : Script(), componentAudio(nullptr), triggerSewersMusicScript(nullptr),
triggerSewersMusicGO(nullptr), audioSourceCantina(nullptr)
{
	REGISTER_FIELD(triggerSewersMusicGO, GameObject*);
	REGISTER_FIELD(audioSourceCantina, ComponentAudioSource*);
}

void TriggerSewersMusicExit::Start()
{
	componentAudio = owner->GetComponent<ComponentAudioSource>();
	triggerSewersMusicScript = triggerSewersMusicGO->GetComponent<TriggerSewersMusic>();
}

void TriggerSewersMusicExit::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player"))
	{
		if (triggerSewersMusicScript->isMusicTriggered)
		{
			audioSourceCantina->PostEvent(AUDIO::SFX::AMBIENT::CANTINA::CANTINA_MUSIC);
			audioSourceCantina->PostEvent(AUDIO::SFX::AMBIENT::CANTINA::CROWD_TALKING_01);
		}

		triggerSewersMusicScript->isMusicTriggered = false;

		AK::SoundEngine::SetState(AUDIO::STATES::GROUP::ZONE, AUDIO::STATES::ID::ZONE::CANTINA);
		AK::SoundEngine::SetState(AUDIO::STATES::GROUP::LIFE, AUDIO::STATES::ID::PLAYERLIFE::ALIVE);
		componentAudio->SetSwitch(AUDIO::MUSIC::SWITCH::GROUP::GAMEPLAY,
			AUDIO::MUSIC::SWITCH::ID::GAMEPLAY::EXPLORATION);

		componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::AMBIENT_STOP);

	}
}