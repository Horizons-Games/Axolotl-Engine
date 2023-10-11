#include "StdAfx.h"
#include "TurbineSewersAudio.h"

#include "Components/ComponentAudioSource.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(TurbineSewersAudio);

TurbineSewersAudio::TurbineSewersAudio() : Script(), componentAudio(nullptr)
{

}

TurbineSewersAudio::~TurbineSewersAudio()
{
}

void TurbineSewersAudio::Start()
{
	componentAudio = owner->GetComponent<ComponentAudioSource>();

	componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::TURBINE);
}