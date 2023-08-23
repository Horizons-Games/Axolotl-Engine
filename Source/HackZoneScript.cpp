#include "StdAfx.h"
#include "HackZoneScript.h"

#include <algorithm>
#include <random>

REGISTERCLASS(HackZoneScript);

HackZoneScript::HackZoneScript() 
	: Script()
{
	REGISTER_FIELD(influenceRadius, float);
	REGISTER_FIELD(maxTime, float);
	REGISTER_FIELD(sequenceSize, int);
}



void HackZoneScript::Start()
{
	keyCombination.reserve(sequenceSize);
	buttonCombination.reserve(sequenceSize);

	GenerateCombination();
}

void HackZoneScript::Update(float deltaTime)
{
}

void HackZoneScript::GenerateCombination()
{
	std::vector<SDL_Scancode> allKeys = { SDL_SCANCODE_A, SDL_SCANCODE_B, SDL_SCANCODE_C };
	std::vector<SDL_GameControllerButton> allButtons = { SDL_CONTROLLER_BUTTON_A, SDL_CONTROLLER_BUTTON_B };

	std::random_device rd;
	std::default_random_engine generator(rd());

	keyCombination.clear();
	buttonCombination.clear();

	for (int i = 0; i < sequenceSize; ++i)
	{
		std::uniform_int_distribution<int> distribution(0, allKeys.size() - 1);
		keyCombination.push_back(allKeys[distribution(generator)]);
	}

	for (int i = 0; i < sequenceSize; ++i)
	{
		std::uniform_int_distribution<int> distribution(0, allButtons.size() - 1);
		buttonCombination.push_back(allButtons[distribution(generator)]);
	}
}
