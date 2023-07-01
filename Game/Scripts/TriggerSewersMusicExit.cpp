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
triggerSewersMusicGO(nullptr)
{
	REGISTER_FIELD(triggerSewersMusicGO, GameObject*);
}

void TriggerSewersMusicExit::Start()
{
	componentAudio = owner->GetComponent<ComponentAudioSource>();
	triggerSewersMusicScript = triggerSewersMusicGO->GetComponent<TriggerSewersMusic>();
}

void TriggerSewersMusicExit::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->GetComponent<ComponentPlayer>())
	{
		triggerSewersMusicScript->isMusicTriggered = false;

		AK::SoundEngine::SetState(AUDIO::STATES::GROUP::ZONE, AUDIO::STATES::ID::ZONE::CANTINA);
		AK::SoundEngine::SetState(AUDIO::STATES::GROUP::LIFE, AUDIO::STATES::ID::PLAYERLIFE::ALIVE);
		componentAudio->SetSwitch(AUDIO::MUSIC::SWITCH::GROUP::GAMEPLAY, AUDIO::MUSIC::SWITCH::ID::GAMEPLAY::EXPLORATION);
	}
}