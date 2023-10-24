#include "StdAfx.h"
#include "UIEnemyBar.h"

#include "../Scripts/HealthSystem.h"

#include "ModulePlayer.h"
#include "Application.h"

#include "Components/ComponentScript.h"
#include "Components/UI/ComponentSlider.h"

REGISTERCLASS(UIEnemyBar);

UIEnemyBar::UIEnemyBar() : Script(), firstChange(60), secondChange(30), thirdChange(10), boss(nullptr), alwaysActive(false)
{
	REGISTER_FIELD(boss, HealthSystem*);
	REGISTER_FIELD(firstChange, float);
	REGISTER_FIELD(secondChange, float);
	REGISTER_FIELD(thirdChange, float);
	REGISTER_FIELD(alwaysActive, bool);
}

UIEnemyBar::~UIEnemyBar()
{
}

void UIEnemyBar::Start()
{
	slider = GetOwner()->GetChildren()[1]->GetComponent<ComponentSlider>();
	slider->SetMaxValue( boss->GetMaxHealth() );
	slider->ModifyCurrentValue(0);
	barValue = 0;
	actualMode = LEVEL1;
	icons = GetOwner()->GetChildren()[2];
}

void UIEnemyBar::Update(float deltaTime)
{
	if (alwaysActive)
	{
		UpdateBar();
		CheckMode();
	}
}

void UIEnemyBar::UpdateBar()
{
	if (static_cast<int>(boss->GetCurrentHealth()) > barValue)
	{
		barValue++;
		if (barValue - static_cast<int>(boss->GetCurrentHealth()) < -static_cast<int>(slider->GetMaxValue()) / 10)
		{
			barValue += 3;
		}
		slider->ModifyCurrentValue(static_cast<float>(barValue));
	}
	else if (static_cast<int>(boss->GetCurrentHealth()) < barValue)
	{
		barValue--;
		slider->ModifyCurrentValue(static_cast<float>(barValue));
	}

}

void UIEnemyBar::CheckMode()
{
	HPMode newMode = LEVEL4;
	if ( static_cast<float>(barValue)/ slider->GetMaxValue() * 100 > firstChange)
	{
		newMode = LEVEL1;
	}
	else if (static_cast<float>(barValue) / slider->GetMaxValue() * 100 > secondChange)
	{
		newMode = LEVEL2;
	}
	else if (static_cast<float>(barValue) / slider->GetMaxValue() * 100 > thirdChange)
	{
		newMode = LEVEL3;
	}
	if (newMode != actualMode)
	{
		actualMode = newMode;
		ChangeMode();
	}
}

void UIEnemyBar::ChangeMode()
{
	switch (actualMode)
	{
	case LEVEL1:
		icons->GetChildren()[0]->Enable();
		icons->GetChildren()[1]->Disable();
		icons->GetChildren()[2]->Disable();
		icons->GetChildren()[3]->Disable();
		break;
	case LEVEL2:
		icons->GetChildren()[0]->Disable();
		icons->GetChildren()[1]->Enable();
		icons->GetChildren()[2]->Disable();
		icons->GetChildren()[3]->Disable();
		break;
	case LEVEL3:
		icons->GetChildren()[0]->Disable();
		icons->GetChildren()[1]->Disable();
		icons->GetChildren()[2]->Enable();
		icons->GetChildren()[3]->Disable();
		break;
	case LEVEL4:
		icons->GetChildren()[0]->Disable();
		icons->GetChildren()[1]->Disable();
		icons->GetChildren()[2]->Disable();
		icons->GetChildren()[3]->Enable();
		break;
	default:
		icons->GetChildren()[0]->Enable();
		icons->GetChildren()[1]->Disable();
		icons->GetChildren()[2]->Disable();
		icons->GetChildren()[3]->Disable();
		break;
	}
}