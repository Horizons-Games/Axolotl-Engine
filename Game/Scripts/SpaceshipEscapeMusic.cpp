#include "StdAfx.h"
#include "SpaceshipEscapeMusic.h"

#include "Components/ComponentAudioSource.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(SpaceshipEscapeMusic);

SpaceshipEscapeMusic::SpaceshipEscapeMusic() : Script(), componentAudio(nullptr)
{

}

SpaceshipEscapeMusic::~SpaceshipEscapeMusic()
{
}

void SpaceshipEscapeMusic::Start()
{
	componentAudio = owner->GetComponent<ComponentAudioSource>();

	AK::SoundEngine::SetState(AUDIO::STATES::GROUP::ZONE, AUDIO::STATES::ID::ZONE::SPACESHIP);
	AK::SoundEngine::SetState(AUDIO::STATES::GROUP::LIFE, AUDIO::STATES::ID::PLAYERLIFE::ALIVE);
	componentAudio->SetSwitch(AUDIO::MUSIC::SWITCH::GROUP::GAMEPLAY, AUDIO::MUSIC::SWITCH::ID::GAMEPLAY::EXPLORATION);

	componentAudio->PostEvent(AUDIO::MUSIC::PLAY_MUSIC);
}