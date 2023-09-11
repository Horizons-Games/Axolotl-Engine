#include "StdAfx.h"
#include "HackZoneScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"

#include "debugdraw.h"

#include <algorithm>
#include <random>

REGISTERCLASS(HackZoneScript);

HackZoneScript::HackZoneScript() 
	: Script(), completed(false)
{
	REGISTER_FIELD(influenceRadius, float);
	REGISTER_FIELD(maxTime, float);
	REGISTER_FIELD(sequenceSize, float);
}

void HackZoneScript::Start()
{
	commandCombination.reserve(sequenceSize);
	position = GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalPosition();
}

void HackZoneScript::Update(float deltaTime)
{
	dd::sphere(position, dd::colors::Blue, influenceRadius);
}

void HackZoneScript::GenerateCombination()
{
	HackingCommandType allCommands[] = 
	{ 
		/*COMMAND_A, 
		COMMAND_B, */
		COMMAND_X, 
		COMMAND_Y 
	};
	size_t allCommandsSize = sizeof(allCommands) / sizeof(allCommands[0]);


	std::random_device rd;
	std::default_random_engine generator(rd());

	commandCombination.clear();

	for (size_t i = 0; i < sequenceSize; ++i)
	{
		std::uniform_int_distribution<size_t> distribution(0, allCommandsSize - 1);
		commandCombination.push_back(allCommands[distribution(generator)]);
	}
}
