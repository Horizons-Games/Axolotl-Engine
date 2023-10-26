#include "MenuMusic.h"

#include "Components/ComponentAudioSource.h"
#include "Application.h"
#include "ModuleAudio.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(MenuMusic);

MenuMusic::MenuMusic() : Script(), componentAudio(nullptr)
{

}

MenuMusic::~MenuMusic()
{
	AK::SoundEngine::SetState(AUDIO::STATES::GROUP::ZONE, AUDIO::STATES::ID::ZONE::CANTINA);
	AK::SoundEngine::SetState(AUDIO::STATES::GROUP::LIFE, AUDIO::STATES::ID::PLAYERLIFE::ALIVE);
	App->GetModule<ModuleAudio>()->SetMusicSwitch(
		AUDIO::MUSIC::SWITCH::GROUP::GAMEPLAY, AUDIO::MUSIC::SWITCH::ID::GAMEPLAY::EXPLORATION);
}

void MenuMusic::Start()
{
	componentAudio = owner->GetComponent<ComponentAudioSource>();

	App->GetModule<ModuleAudio>()->SetMusicID(componentAudio->GetID());

	AK::SoundEngine::SetState(AUDIO::STATES::GROUP::ZONE, AUDIO::STATES::ID::ZONE::NONE);
	AK::SoundEngine::SetState(AUDIO::STATES::GROUP::LIFE, AUDIO::STATES::ID::PLAYERLIFE::NONE);
	App->GetModule<ModuleAudio>()->SetMusicSwitch(
		AUDIO::MUSIC::SWITCH::GROUP::GAMEPLAY, AUDIO::MUSIC::SWITCH::ID::GAMEPLAY::MAIN_MENU);

	componentAudio->PostEvent(AUDIO::MUSIC::PLAY_MUSIC);
}