#pragma once

#include "Scripting\Script.h"
#include <vector>

#include "HackingCommand.h"

class HackZoneScript : public Script
{
public:
	HackZoneScript();
	~HackZoneScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	float GetInfluenceRadius() const;
	float GetMaxTime() const;
	float GetSequenceSize() const;

	bool IsCompleted() const;
	void SetCompleted();

	const std::vector<HackingCommandType>& GetCommandCombination() const;
	
	void GenerateCombination();

private:
	float influenceRadius;
	float maxTime;
	float sequenceSize;
	bool completed;

	std::vector<HackingCommandType> commandCombination;

	float3 position;
};
