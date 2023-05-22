#include "CantinaMusicScript.h"

#include "Components/ComponentAudioSource.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(CantinaMusicScript);

CantinaMusicScript::CantinaMusicScript() : Script(), cantinaAudio(nullptr)
{

}

void CantinaMusicScript::Start()
{
	cantinaAudio = owner->GetComponent<ComponentAudioSource>();

	AK::SoundEngine::SetState(audio::STATE_GROUP_ZONE, audio::STATE_ID_ZONE_CANTINA);
	AK::SoundEngine::SetState(audio::STATE_GROUP_LIFE, audio::STATE_ID_LIFE_ALIVE);
	cantinaAudio->SetSwitch(audio::SWITCH_GROUP_MUSIC_GAMEPLAY, audio::SWITCH_ID_MUSIC_EXPLORATION);

	cantinaAudio->PostEvent(audio::CANTINA_MUSIC);
	cantinaAudio->PostEvent(audio::SFX_AMBIENT_CROWD_TALKING_01);
}