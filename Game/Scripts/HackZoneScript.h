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

inline float HackZoneScript::GetInfluenceRadius() const
{
	return influenceRadius;
}

inline float HackZoneScript::GetMaxTime() const
{
	return maxTime;
}

inline float HackZoneScript::GetSequenceSize() const
{
	return sequenceSize;
}

inline const std::vector<HackingCommandType>& HackZoneScript::GetCommandCombination() const
{
	return commandCombination;
}

inline bool HackZoneScript::IsCompleted() const
{
	return completed;
}

inline void HackZoneScript::SetCompleted()
{
	completed = true;
}

