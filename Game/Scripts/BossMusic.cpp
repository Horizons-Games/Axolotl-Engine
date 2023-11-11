#include "StdAfx.h"
#include "BossMusic.h"

#include "Application.h"

#include "Components/ComponentAudioSource.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(BossMusic);

BossMusic::BossMusic() : Script(), componentAudio(nullptr)
{

}

BossMusic::~BossMusic()
{
}

void BossMusic::Start()
{
	componentAudio = owner->GetComponent<ComponentAudioSource>();
	App->GetModule<ModuleAudio>()->SetMusicID(componentAudio->GetID());

	AK::SoundEngine::SetState(AUDIO::STATES::GROUP::ZONE, AUDIO::STATES::ID::ZONE::BOSS);
	AK::SoundEngine::SetState(AUDIO::STATES::GROUP::LIFE, AUDIO::STATES::ID::PLAYERLIFE::ALIVE);
	App->GetModule<ModuleAudio>()->SetMusicSwitch(
		AUDIO::MUSIC::SWITCH::GROUP::GAMEPLAY, AUDIO::MUSIC::SWITCH::ID::GAMEPLAY::EXPLORATION);

	componentAudio->PostEvent(AUDIO::MUSIC::PLAY_MUSIC_PART02);
}