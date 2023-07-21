#include "StdAfx.h"
#include "ComboManager.h"

#include "Components/ComponentScript.h"
#include "Modules/ModuleInput.h"
#include "UIComboManager.h"

REGISTERCLASS(ComboManager);

ComboManager::ComboManager() : Script(), 
	input(nullptr), 
	specialActivated(false), 
	specialCount(0),
	maxSpecialCount(100),
	comboCount(0), 
	maxComboCount(3),
	comboManagerUIReference(nullptr),
	comboTime(10),
	actualComboTimer(0)
{
	REGISTER_FIELD(comboManagerUIReference, GameObject*);
	REGISTER_FIELD(comboTime, float);
}

void ComboManager::Start()
{
	input = App->GetModule<ModuleInput>();
	uiComboManager = comboManagerUIReference->GetComponent<UIComboManager>();
}

void ComboManager::Update(float deltaTime)
{
	
}

int ComboManager::GetcomboCount() const
{
	return comboCount;
}

void ComboManager::CheckSpecial(float deltaTime)
{
	if (input->GetKey(SDL_SCANCODE_TAB) == KeyState::DOWN && specialCount == maxSpecialCount)
	{
		specialActivated = true;
		uiComboManager->SetActivateSpecial(true);
		//ADD UI & PARTICLE CALLS
	}

	if (actualComboTimer <= 0)
	{
		if (comboCount > 0)
		{
			comboCount = 0;
			uiComboManager->ClearCombo(false);
			actualComboTimer = comboTime;
		}
		else if (specialCount > 0 && specialCount < maxSpecialCount)
		{
			specialCount = std::max(0.0f, specialCount - (5 * deltaTime));
			uiComboManager->SetComboBarValue(specialCount);
		}
	}
	else
	{
		actualComboTimer -= deltaTime;
	}
}

bool ComboManager::isSpecialActivated() const
{
	return specialActivated;
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

void ComboManager::SuccessfulAttack(float specialCount, AttackType type)
{
	if(specialCount < 0 || !specialActivated)
	{
		this->specialCount = std::max(0.0f, std::min(this->specialCount + specialCount, maxSpecialCount));
		if (this->specialCount <= 0.0f && specialActivated)
		{
			specialActivated = false;
		}
		uiComboManager->SetComboBarValue(this->specialCount);
		actualComboTimer = comboTime;
	}

	comboCount++;
	if(type == AttackType::HEAVYNORMAL || type == AttackType::HEAVYFINISHER)
	{
		uiComboManager->AddInputVisuals(InputVisualType::HEAVY);
	}
	else if (type == AttackType::SOFTNORMAL || type == AttackType::SOFTFINISHER)
	{
		uiComboManager->AddInputVisuals(InputVisualType::SOFT);
	}

	if (comboCount == 3 || type == AttackType::JUMPATTACK) 
	{
		uiComboManager->ClearCombo(true);
		comboCount = 0;
	}
}