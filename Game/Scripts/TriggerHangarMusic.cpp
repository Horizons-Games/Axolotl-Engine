#include "StdAfx.h"
#include "TriggerHangarMusic.h"

#include "Application.h"

#include "Components/ComponentAudioSource.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentPlayer.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(TriggerHangarMusic);

TriggerHangarMusic::TriggerHangarMusic() : Script()
{
}

void TriggerHangarMusic::Start()
{
}

void TriggerHangarMusic::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player"))
	{
		AK::SoundEngine::SetState(AUDIO::STATES::GROUP::ZONE, AUDIO::STATES::ID::ZONE::HANGAR);
	}
}