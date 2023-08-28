#pragma once

#include "Scripting\Script.h"
#include <vector>

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

	bool GetCompleted() const;
	void SetCompleted();

	const std::vector<SDL_Scancode>& GetKeyCombination() const;
	const std::vector<SDL_GameControllerButton>& GetButtonCombination() const;
	
	void GenerateCombination();

private:

	float influenceRadius;
	float maxTime;
	float sequenceSize;
	bool completed;

	std::vector<SDL_Scancode> keyCombination;
	std::vector<SDL_GameControllerButton> buttonCombination;

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

inline const std::vector<SDL_Scancode>& HackZoneScript::GetKeyCombination() const
{
	return keyCombination;
}
inline const std::vector<SDL_GameControllerButton>& HackZoneScript::GetButtonCombination() const
{
	return buttonCombination;
}

inline bool HackZoneScript::GetCompleted() const
{
	return completed;
}

inline void HackZoneScript::SetCompleted()
{
	completed = true;
}

