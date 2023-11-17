#include "TriggerSewersMusicExit.h"

#include "Application.h"

#include "Components/ComponentAudioSource.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentPlayer.h"
#include "../Scripts/TriggerSewersMusic.h"

#include "GameObject/GameObject.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(TriggerSewersMusicExit);

TriggerSewersMusicExit::TriggerSewersMusicExit() : Script(), triggerSewersMusicScript(nullptr),
triggerSewersMusicGO(nullptr)
{
	REGISTER_FIELD(triggerSewersMusicGO, GameObject*);
}

void TriggerSewersMusicExit::Start()
{
	triggerSewersMusicScript = triggerSewersMusicGO->GetComponent<TriggerSewersMusic>();
}

void TriggerSewersMusicExit::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player"))
	{
		triggerSewersMusicScript->isTriggered = false;

		AK::SoundEngine::SetState(AUDIO::STATES::GROUP::ZONE, AUDIO::STATES::ID::ZONE::CANTINA);

		triggerSewersMusicScript->componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::AMBIENT_STOP);
	}
}