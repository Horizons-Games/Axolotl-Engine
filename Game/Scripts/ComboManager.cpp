#include "StdAfx.h"
#include "ComboManager.h"

#include "Application.h"

#include "Components/ComponentScript.h"
#include "UIComboManager.h"
#include "CameraControllerScript.h"
#include "ModulePlayer.h"

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
	if (!uiComboManager->IsInCombat() && App->GetModule<ModulePlayer>()->IsInCombat()) 
	{
		InitCombo();
	}
	else if (uiComboManager->IsInCombat() && !App->GetModule<ModulePlayer>()->IsInCombat())
	{
		HideCombo();
	}

	if (!specialActivated && specialCount == maxSpecialCount)
	{
		specialActivated = true;

		uiComboManager->SetActivateSpecial(true);
		actualComboTimer = comboTime;
		uiComboManager->UpdateFadeOut(1.0f);
		ClearCombo(false);
	}

	if (actualComboTimer <= 0.0f)
	{
		if (comboCount > 0)
		{
			ClearCombo(false);
			if (!specialActivated)
			{
				actualComboTimer = comboTime;
			}
		}
		else if (specialCount > 0 && specialCount < maxSpecialCount && !specialActivated)
		{
			specialCount = std::max(0.0f, specialCount - 5.0f * deltaTime);
			uiComboManager->SetComboBarValue(specialCount);
		}
	}
	else if (comboCount > 0)
	{
		uiComboManager->UpdateFadeOut(actualComboTimer / comboTime);
		actualComboTimer -= deltaTime;
		if (actualComboTimer < 1.0f)
		{
			actualComboTimer = 0.0f;
		}
	}
}

void ComboManager::ClearCombo(bool finisher) 
{
	uiComboManager->ClearCombo(finisher);
	comboCount = 0;
}

void ComboManager::ClearComboForSwitch(bool finisher) 
{
	uiComboManager->ClearCombo(finisher);
	uiComboManager->SetComboBarValue(0);
	comboCount = 0;
	specialCount = 0.f;
}

AttackType ComboManager::CheckAttackInput(bool jumping)
{
	bool leftClick = input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::DOWN;
	bool rightClick = input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::DOWN;
	bool lightSpecialInput = input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::DOWN;
	bool heavySpecialInput = input->GetKey(SDL_SCANCODE_Z) == KeyState::DOWN;

	if (jumping && (leftClick || rightClick))
	{
		if (specialActivated)
		{
			return AttackType::JUMPFINISHER;
		}
		return AttackType::JUMPNORMAL;
	}

	if (leftClick)
	{
		return AttackType::LIGHTNORMAL;
	}

	if (rightClick) 
	{
		return AttackType::HEAVYNORMAL;
	}

	if (specialActivated) 
	{
		if (lightSpecialInput) 
		{
			return AttackType::LIGHTFINISHER;
		}

		if (heavySpecialInput) 
		{
			return AttackType::HEAVYFINISHER;
		}
	}

	return AttackType::NONE;
}

void ComboManager::SuccessfulAttack(float specialCount, AttackType type)
{
	actualComboTimer = comboTime;
	uiComboManager->UpdateFadeOut(1.0f);

	comboCount++;

	uiComboManager->AddInputVisuals(type);

	if (comboCount == 3 || type == AttackType::JUMPNORMAL || type == AttackType::JUMPFINISHER 
		|| type == AttackType::HEAVYFINISHER || type == AttackType::LIGHTFINISHER)
	{
		ClearCombo(true);
	}

	if (specialCount < 0 || !specialActivated)
	{
		this->specialCount =
			std::clamp(this->specialCount + specialCount, 0.0f, maxSpecialCount);

		if (this->specialCount <= 0.0f && specialActivated)
		{
			specialActivated = false;
		}

		uiComboManager->SetComboBarValue(this->specialCount);

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

UIComboManager* ComboManager::GetUiComboManager() const
{
	return uiComboManager;
}

void ComboManager::InitCombo()
{
	if (!uiComboManager->IsInCombat()) 
	{
		specialCount = 0.0f;
		comboCount = 0;
		actualComboTimer = 0.0f;
		specialActivated = false;

		uiComboManager->InitComboUI();
	}
}

void ComboManager::HideCombo()
{
	if (uiComboManager->IsInCombat()) 
	{
		specialCount = 0.0f;
		comboCount = 0;
		actualComboTimer = 0.0f;
		specialActivated = false;

		uiComboManager->HideComboUI();
	}
}
