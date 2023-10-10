#include "StdAfx.h"
#include "WaterfallAudio.h"

#include "Components/ComponentAudioSource.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(WaterfallAudio);

WaterfallAudio::WaterfallAudio() : Script(), componentAudio(nullptr)
{

}

WaterfallAudio::~WaterfallAudio()
{
}

void WaterfallAudio::Start()
{
	componentAudio = owner->GetComponent<ComponentAudioSource>();

	componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::WATER_FALLING);
}