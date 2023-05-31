#include "CantinaNPCAnimationManager.h"
#include "Components/ComponentAnimation.h"

REGISTERCLASS(CantinaNPCAnimationManager);

CantinaNPCAnimationManager::CantinaNPCAnimationManager() : Script(), animation(nullptr), activation(false), loopAnimation(false), percentageOfActivation(0)
{
	REGISTER_FIELD(loopAnimation, bool);
	REGISTER_FIELD(percentageOfActivation, float);
}

void CantinaNPCAnimationManager::Start()
{
	animation = static_cast<ComponentAnimation*>(owner->GetComponent(ComponentType::ANIMATION));
	std::srand(std::time(NULL));
}

void CantinaNPCAnimationManager::Update(float deltaTime)
{
	if(!loopAnimation || !activation)
	{
		if (activation)
		{
			animation->SetParameter("Activate", false);
			activation = false;
		}

		int randomValue = std::rand() % 100 + 1;

		if(randomValue <= percentageOfActivation)
		{
			animation->SetParameter("Activate", true);
			activation = true;
		}
	}
}