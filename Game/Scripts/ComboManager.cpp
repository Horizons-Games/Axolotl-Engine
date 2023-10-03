#include "StdAfx.h"
#include "ComboManager.h"

#include "Application.h"

#include "Components/ComponentScript.h"
#include "UIComboManager.h"

REGISTERCLASS(ComboManager);

ComboManager::ComboManager() : Script(), 
	input(nullptr), 
	specialActivated(false), 
	specialCount(0.0f),
	maxSpecialCount(100.0f),
	comboCount(0), 
	maxComboCount(3.0f),
	uiComboManager(nullptr),
	comboTime(10.0f),
	actualComboTimer(0.0f)
{
	REGISTER_FIELD(uiComboManager, UIComboManager*);
	REGISTER_FIELD(comboTime, float);
	REGISTER_FIELD(maxComboCount, float);
}

void ComboManager::Init()
{
	Assert(uiComboManager != nullptr, axo::Format("UIComboManager combo manager not set!! Owner: {}", GetOwner()));
}

void ComboManager::Start()
{
	input = App->GetModule<ModuleInput>();

	maxSpecialCount = uiComboManager->GetMaxComboBarValue();
	uiComboManager->SetComboBarValue(specialCount);
}

int ComboManager::GetComboCount() const
{
	return comboCount;
}

bool ComboManager::NextIsSpecialAttack() const
{
	return specialActivated && comboCount == maxComboCount - 1;
}

void ComboManager::CheckSpecial(float deltaTime)
{
	if (input->GetKey(SDL_SCANCODE_TAB) == KeyState::DOWN && specialCount == maxSpecialCount)
	{
		specialActivated = true;

		uiComboManager->SetActivateSpecial(true);

		ClearCombo(false);
	}

	if (actualComboTimer <= 0)
	{
		if (comboCount > 0)
		{
			ClearCombo(false);
			actualComboTimer = comboTime;
		}

		else if (specialCount > 0 && specialCount < maxSpecialCount)
		{
			specialCount = std::max(0.0f, specialCount - 5.0f * deltaTime);

			uiComboManager->SetComboBarValue(specialCount);
		}
	}

	else
	{
		actualComboTimer -= deltaTime;
		if (uiComboManager)
		{
			uiComboManager->UpdateFadeOut(actualComboTimer / comboTime);
		}
	}
}

void ComboManager::ClearCombo(bool finisher) 
{
	uiComboManager->ClearCombo(finisher);
	comboCount = 0;
}

AttackType ComboManager::CheckAttackInput(bool jumping)
{
	bool leftClick = input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::DOWN;
	bool rightClick = input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::DOWN;

	if (jumping && (leftClick || rightClick))
	{
		if (specialActivated && comboCount == maxComboCount - 1)
		{
			return AttackType::JUMPFINISHER;
		}

		return AttackType::JUMPNORMAL;
	}

	if (leftClick)
	{
		if (specialActivated && comboCount == maxComboCount - 1)
		{
			return AttackType::LIGHTFINISHER;
		}

		return AttackType::LIGHTNORMAL;
	}

	if (rightClick)
	{
		if (specialActivated && comboCount == maxComboCount - 1)
		{
			return AttackType::HEAVYFINISHER;
		}

		return AttackType::HEAVYNORMAL;
	}

	return AttackType::NONE;
}

void ComboManager::SuccessfulAttack(float specialCount, AttackType type)
{
	if (specialCount < 0 || !specialActivated)
	{
		this->specialCount = 
			std::clamp(this->specialCount + specialCount, 0.0f, maxSpecialCount);

		if (this->specialCount <= 0.0f && specialActivated)
		{
			specialActivated = false;
		}

		uiComboManager->SetComboBarValue(this->specialCount);

		actualComboTimer = comboTime;
	}

	comboCount++;
	if (type == AttackType::HEAVYNORMAL || type == AttackType::HEAVYFINISHER)
	{
		uiComboManager->AddInputVisuals(InputVisualType::HEAVY);
	}

	else if (type == AttackType::LIGHTNORMAL || type == AttackType::LIGHTFINISHER)
	{
		uiComboManager->AddInputVisuals(InputVisualType::LIGHT);
	}

	if (comboCount == 3 || type == AttackType::JUMPNORMAL) 
	{
		ClearCombo(true);
	}
}

bool ComboManager::IsSpecialActivated() const
{
	return specialActivated;
}

void ComboManager::FillComboBar()
{
	specialCount = maxSpecialCount;
	uiComboManager->SetComboBarValue(specialCount);
}