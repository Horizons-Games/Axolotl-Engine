#include "TriggerSewersMusic.h"

#include "Application.h"

#include "Components/ComponentAudioSource.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentPlayer.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(TriggerSewersMusic);

TriggerSewersMusic::TriggerSewersMusic() : Script(), componentAudio(nullptr), isMusicTriggered(false),
audioSourceCantina(nullptr)
{
	REGISTER_FIELD(audioSourceCantina, ComponentAudioSource*);
}

void TriggerSewersMusic::Start()
{
	componentAudio = owner->GetComponent<ComponentAudioSource>();

	App->GetModule<ModuleAudio>()->SetMusicID(componentAudio->GetID());
}

void TriggerSewersMusic::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player") && !isMusicTriggered)
	{
		AK::SoundEngine::SetState(AUDIO::STATES::GROUP::ZONE, AUDIO::STATES::ID::ZONE::SEWERS);
		AK::SoundEngine::SetState(AUDIO::STATES::GROUP::LIFE, AUDIO::STATES::ID::PLAYERLIFE::ALIVE);
		App->GetModule<ModuleAudio>()->SetMusicSwitch(AUDIO::MUSIC::SWITCH::GROUP::GAMEPLAY,
			AUDIO::MUSIC::SWITCH::ID::GAMEPLAY::EXPLORATION);

		if (!isMusicTriggered)
		{
			componentAudio->PostEvent(AUDIO::MUSIC::PLAY_MUSIC);
			audioSourceCantina->PostEvent(AUDIO::SFX::AMBIENT::CANTINA::STOP_AMBIENT_CANTINA);
			componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::AMBIENT);
		}
		isMusicTriggered = true;
	}
}