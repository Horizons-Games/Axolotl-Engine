#include "ComboManager.h"

#include "Components/ComponentScript.h"
#include "Modules/ModuleInput.h"

REGISTERCLASS(ComboManager);

ComboManager::ComboManager() : Script(), 
	input(nullptr), 
	specialActivated(false), 
	specialCount(0),
	maxSpecialCount(100),
	comboCount(0), 
	maxComboCount(3)
{
}

void ComboManager::Start()
{
	input = App->GetModule<ModuleInput>();
}

void ComboManager::Update(float deltaTime)
{
}

void ComboManager::CheckSpecial()
{
	if (input->GetKey(SDL_SCANCODE_TAB) == KeyState::DOWN && specialCount == maxSpecialCount)
	{
		specialActivated = true;
		//ADD UI & PARTICLE CALLS
	}
	//CHECK TIME WITHOUT COMBO AND GO DOWN
}

AttackType ComboManager::CheckAttackInput(bool jumping)
{
	bool leftClick = input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::DOWN;
	bool rightClick = input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::DOWN;

	if (jumping && (leftClick || rightClick))
	{
		return AttackType::JUMPATTACK;
	}

	if (leftClick)
	{
		if (specialActivated && comboCount == maxComboCount - 1)
		{
			return AttackType::SOFTFINISHER;
		}
		return AttackType::SOFTNORMAL;
	}

	if (rightClick)
	{
		if (specialActivated && comboCount == maxComboCount -1)
		{
			return AttackType::HEAVYFINISHER;
		}
		return AttackType::HEAVYNORMAL;
	}

	return AttackType::NONE;
}

void ComboManager::SuccessfulAttack(int specialCount, bool heavy)
{
	this->specialCount = std::max(0, std::min(this->specialCount + specialCount, maxComboCount));
	comboCount++;
	//ADD UI CALL
}