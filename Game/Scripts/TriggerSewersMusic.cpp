#include "TriggerSewersMusic.h"

#include "Components/ComponentAudioSource.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentPlayer.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(TriggerSewersMusic);

TriggerSewersMusic::TriggerSewersMusic() : Script(), componentAudio(nullptr), isMusicTriggered(false)
{

}

void TriggerSewersMusic::Start()
{
	componentAudio = owner->GetComponent<ComponentAudioSource>();

}

void TriggerSewersMusic::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->GetComponent<ComponentPlayer>())
	{
		if (!isMusicTriggered)
		{
			AK::SoundEngine::SetState(AUDIO::STATES::GROUP::ZONE, AUDIO::STATES::ID::ZONE::SEWERS);
			AK::SoundEngine::SetState(AUDIO::STATES::GROUP::LIFE, AUDIO::STATES::ID::PLAYERLIFE::ALIVE);
			componentAudio->SetSwitch(AUDIO::MUSIC::SWITCH::GROUP::GAMEPLAY, AUDIO::MUSIC::SWITCH::ID::GAMEPLAY::EXPLORATION);

			componentAudio->PostEvent(AUDIO::MUSIC::PLAY_MUSIC);
			isMusicTriggered = true;
		}
	}
}