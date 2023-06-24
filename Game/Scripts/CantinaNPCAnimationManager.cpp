#include "CantinaNPCAnimationManager.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentAudioSource.h"
#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(CantinaNPCAnimationManager);

CantinaNPCAnimationManager::CantinaNPCAnimationManager() : Script(), audio(nullptr), animation(nullptr), activation(false), loopAnimation(false), priority(0)
{
	REGISTER_FIELD(loopAnimation, bool);
	REGISTER_FIELD(priority, float);
}

void CantinaNPCAnimationManager::Start()
{
	audio = owner->GetComponent<ComponentAudioSource>();

	animation = owner->GetComponent<ComponentAnimation>();
}

void CantinaNPCAnimationManager::Update(float deltaTime)
{
	if(!loopAnimation) 
	{
		if (!animation->isPlaying())
		{
			animation->SetParameter("Activate", false);
			activation = false;
		}

		int randomValue = std::rand() % 100 + 1;

		if (!activation && randomValue <= priority)
		{
			if (audio)
			{
				audio->PostEvent(AUDIO::SFX::NPC::CANTINA::DRUNKNPC_RANDOM_SOUND);
			}

			animation->SetParameter("Activate", true);
			activation = true;
		}
	}
	else if(!activation)
	{
		timer += deltaTime;
		if(priority <= timer)
		{
			animation->SetParameter("Activate", true);
		}
	}
}