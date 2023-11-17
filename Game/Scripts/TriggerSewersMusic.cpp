#include "TriggerSewersMusic.h"

#include "Application.h"

#include "Components/ComponentAudioSource.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentPlayer.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(TriggerSewersMusic);

TriggerSewersMusic::TriggerSewersMusic() : Script(), isTriggered(false),
componentAudio(nullptr)
{
}

void TriggerSewersMusic::Start()
{
	componentAudio = owner->GetComponent<ComponentAudioSource>();
}

void TriggerSewersMusic::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player") && !isTriggered)
	{
		AK::SoundEngine::SetState(AUDIO::STATES::GROUP::ZONE, AUDIO::STATES::ID::ZONE::SEWERS);

		if (!isTriggered)
		{
			componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::AMBIENT);
		}
		isTriggered = true;
	}
}