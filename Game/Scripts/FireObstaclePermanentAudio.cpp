#include "StdAfx.h"
#include "FireObstaclePermanentAudio.h"

#include "Components/ComponentAudioSource.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(FireObstaclePermanentAudio);

FireObstaclePermanentAudio::FireObstaclePermanentAudio() : Script(), componentAudio(nullptr)
{

}

FireObstaclePermanentAudio::~FireObstaclePermanentAudio()
{
}

void FireObstaclePermanentAudio::Start()
{
	componentAudio = owner->GetComponent<ComponentAudioSource>();

	componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SPACESHIPESCAPE::FIRE);
}