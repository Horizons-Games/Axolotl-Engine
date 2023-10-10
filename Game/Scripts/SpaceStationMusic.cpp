#include "StdAfx.h"
#include "SpaceStationMusic.h"

#include "Components/ComponentAudioSource.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(SpaceStationMusic);

SpaceStationMusic::SpaceStationMusic() : Script(), componentAudio(nullptr)
{

}

SpaceStationMusic::~SpaceStationMusic()
{
}

void SpaceStationMusic::Start()
{
	componentAudio = owner->GetComponent<ComponentAudioSource>();

	AK::SoundEngine::SetState(AUDIO::STATES::GROUP::ZONE, AUDIO::STATES::ID::ZONE::SPACESTATION);
	AK::SoundEngine::SetState(AUDIO::STATES::GROUP::LIFE, AUDIO::STATES::ID::PLAYERLIFE::ALIVE);
	componentAudio->SetSwitch(AUDIO::MUSIC::SWITCH::GROUP::GAMEPLAY, AUDIO::MUSIC::SWITCH::ID::GAMEPLAY::EXPLORATION);

	componentAudio->PostEvent(AUDIO::MUSIC::PLAY_MUSIC);
}