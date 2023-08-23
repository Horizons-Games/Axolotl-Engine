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

	const std::vector<SDL_Scancode>& GetKeyCombination() const;
	const std::vector<SDL_GameControllerButton>& GetButtonCombination() const;

private:

	void GenerateCombination();

private:

	float influenceRadius;
	float maxTime;
	int sequenceSize;

	std::vector<SDL_Scancode> keyCombination;
	std::vector<SDL_GameControllerButton> buttonCombination;
};

inline float HackZoneScript::GetInfluenceRadius() const
{
	return influenceRadius;
}

inline float HackZoneScript::GetMaxTime() const
{
	return maxTime;
}

inline const std::vector<SDL_Scancode>& HackZoneScript::GetKeyCombination() const
{
	return keyCombination;
}
inline const std::vector<SDL_GameControllerButton>& HackZoneScript::GetButtonCombination() const
{
	return buttonCombination;
}

