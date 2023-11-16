#include "StdAfx.h"
#include "TurbineSpaceshipEscapeAudio.h"

#include "Components/ComponentAudioSource.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(TurbineSpaceshipEscapeAudio);

TurbineSpaceshipEscapeAudio::TurbineSpaceshipEscapeAudio() : Script(), componentAudio(nullptr)
{

}

TurbineSpaceshipEscapeAudio::~TurbineSpaceshipEscapeAudio()
{
}

void TurbineSpaceshipEscapeAudio::Start()
{
	componentAudio = owner->GetComponent<ComponentAudioSource>();

	componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SPACESHIPESCAPE::TURBINE);
}