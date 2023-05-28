#include "CantinaMusicScript.h"

#include "Components/ComponentAudioSource.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(CantinaMusicScript);

CantinaMusicScript::CantinaMusicScript() : Script(), cantinaAudio(nullptr)
{

}

void CantinaMusicScript::Start()
{
	cantinaAudio = static_cast<ComponentAudioSource*>(owner->GetComponent(ComponentType::AUDIOSOURCE));

	AK::SoundEngine::SetState(AUDIO::STATES::GROUP::ZONE, AUDIO::STATES::ID::ZONE::CANTINA);
	AK::SoundEngine::SetState(AUDIO::STATES::GROUP::LIFE, AUDIO::STATES::ID::PLAYERLIFE::ALIVE);
	cantinaAudio->SetSwitch(AUDIO::MUSIC::SWITCH::GROUP::GAMEPLAY, AUDIO::MUSIC::SWITCH::ID::GAMEPLAY::EXPLORATION);

	cantinaAudio->PostEvent(AUDIO::SFX::AMBIENT::CANTINA::CANTINA_MUSIC);
	cantinaAudio->PostEvent(AUDIO::SFX::AMBIENT::CANTINA::CROWD_TALKING_01);
}