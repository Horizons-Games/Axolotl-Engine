#include "StdAfx.h"
#include "GeneratorSpaceStationAudio.h"

#include "Components/ComponentAudioSource.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(GeneratorSpaceStationAudio);

GeneratorSpaceStationAudio::GeneratorSpaceStationAudio() : Script(), componentAudio(nullptr)
{

}

GeneratorSpaceStationAudio::~GeneratorSpaceStationAudio()
{
}

void GeneratorSpaceStationAudio::Start()
{
	componentAudio = owner->GetComponent<ComponentAudioSource>();

	componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SPACESTATION::GENERATOR);
}