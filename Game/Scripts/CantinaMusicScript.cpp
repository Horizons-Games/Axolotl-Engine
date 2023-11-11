#include "CantinaMusicScript.h"

#include "Application.h"

#include "Components/ComponentAudioSource.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(CantinaMusicScript);

CantinaMusicScript::CantinaMusicScript() : Script(), cantinaAudio(nullptr)
{

}

void CantinaMusicScript::Start()
{
	cantinaAudio = owner->GetComponent<ComponentAudioSource>();

	App->GetModule<ModuleAudio>()->SetMusicID(cantinaAudio->GetID());

	AK::SoundEngine::SetState(AUDIO::STATES::GROUP::ZONE, AUDIO::STATES::ID::ZONE::CANTINA);
	AK::SoundEngine::SetState(AUDIO::STATES::GROUP::LIFE, AUDIO::STATES::ID::PLAYERLIFE::ALIVE);
	App->GetModule<ModuleAudio>()->SetMusicSwitch(AUDIO::MUSIC::SWITCH::GROUP::GAMEPLAY,
		AUDIO::MUSIC::SWITCH::ID::GAMEPLAY::EXPLORATION);

	cantinaAudio->PostEvent(AUDIO::MUSIC::PLAY_MUSIC);
	cantinaAudio->PostEvent(AUDIO::SFX::AMBIENT::CANTINA::CROWD_TALKING_01);
}