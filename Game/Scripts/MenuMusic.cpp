#include "MenuMusic.h"

#include "Components/ComponentAudioSource.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(MenuMusic);

MenuMusic::MenuMusic() : Script(), componentAudio(nullptr)
{

}

MenuMusic::~MenuMusic()
{
	AK::SoundEngine::SetState(AUDIO::STATES::GROUP::ZONE, AUDIO::STATES::ID::ZONE::CANTINA);
	AK::SoundEngine::SetState(AUDIO::STATES::GROUP::LIFE, AUDIO::STATES::ID::PLAYERLIFE::ALIVE);
	componentAudio->SetSwitch(AUDIO::MUSIC::SWITCH::GROUP::GAMEPLAY, AUDIO::MUSIC::SWITCH::ID::GAMEPLAY::EXPLORATION);
}

void MenuMusic::Start()
{
	componentAudio = static_cast<ComponentAudioSource*>(owner->GetComponent(ComponentType::AUDIOSOURCE));

	AK::SoundEngine::SetState(AUDIO::STATES::GROUP::ZONE, AUDIO::STATES::ID::ZONE::NONE);
	AK::SoundEngine::SetState(AUDIO::STATES::GROUP::LIFE, AUDIO::STATES::ID::PLAYERLIFE::NONE);
	componentAudio->SetSwitch(AUDIO::MUSIC::SWITCH::GROUP::GAMEPLAY, AUDIO::MUSIC::SWITCH::ID::GAMEPLAY::MAIN_MENU);

	componentAudio->PostEvent(AUDIO::MUSIC::PLAY_MUSIC);
}