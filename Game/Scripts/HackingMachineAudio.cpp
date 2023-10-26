#include "StdAfx.h"
#include "HackingMachineAudio.h"

#include "Components/ComponentAudioSource.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(HackingMachineAudio);

HackingMachineAudio::HackingMachineAudio() : Script(), componentAudio(nullptr)
{

}

HackingMachineAudio::~HackingMachineAudio()
{
}

void HackingMachineAudio::Start()
{
	componentAudio = owner->GetComponent<ComponentAudioSource>();

	componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SPACESTATION::HACKING);
}