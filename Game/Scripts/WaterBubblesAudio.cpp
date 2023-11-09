#include "StdAfx.h"
#include "WaterBubblesAudio.h"

#include "Components/ComponentAudioSource.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(WaterBubblesAudio);

WaterBubblesAudio::WaterBubblesAudio() : Script(), componentAudio(nullptr)
{

}

WaterBubblesAudio::~WaterBubblesAudio()
{
}

void WaterBubblesAudio::Start()
{
	componentAudio = owner->GetComponent<ComponentAudioSource>();

	componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BUBBLES);
}