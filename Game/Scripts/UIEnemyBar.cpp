#include "StdAfx.h"
#include "UIEnemyBar.h"

#include "../Scripts/HealthSystem.h"
#include "../Scripts/UIApearDisapear.h"
#include "../Scripts/UIImageDisplacementControl.h"

#include "ModulePlayer.h"
#include "Application.h"

#include "Components/ComponentScript.h"
#include "Components/UI/ComponentSlider.h"

REGISTERCLASS(UIEnemyBar);

UIEnemyBar::UIEnemyBar() : Script(), firstChange(60), secondChange(30), thirdChange(10), boss(nullptr), alwaysActive(false), appearNextCombat(true)
{
	REGISTER_FIELD(boss, HealthSystem*);
	REGISTER_FIELD(firstChange, float);
	REGISTER_FIELD(secondChange, float);
	REGISTER_FIELD(thirdChange, float);
	REGISTER_FIELD(alwaysActive, bool);
	REGISTER_FIELD(appearNextCombat, bool);
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
	actualMode = LEVEL4;
	icons = GetOwner()->GetChildren()[2];
	if (!alwaysActive)
	{
		displacement = GetOwner()->GetComponent<UIImageDisplacementControl>();
	}
}

void UIEnemyBar::Update(float deltaTime)
{
	if (alwaysActive || App->GetModule<ModulePlayer>()->IsInCombat())
	{
		UpdateBar();
		CheckMode();
	}
	if (appearNextCombat && !alwaysActive && lastTickInCombat != App->GetModule<ModulePlayer>()->IsInCombat())
	{
		appearNextCombat = false;
		lastTickInCombat = App->GetModule<ModulePlayer>()->IsInCombat();
		if (lastTickInCombat)
		{
			displacement->SetMovingToEnd(true);
			displacement->MoveImageToEndPosition();
		}
		else
		{
			displacement->SetMovingToEnd(false);
			displacement->MoveImageToStartPosition();
		}
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
		icons->GetChildren()[0]->GetComponent<UIApearDisapear>()->SetObjective(1.f);
		icons->GetChildren()[1]->GetComponent<UIApearDisapear>()->SetObjective(0.f);
		icons->GetChildren()[2]->GetComponent<UIApearDisapear>()->SetObjective(0.f);
		icons->GetChildren()[3]->GetComponent<UIApearDisapear>()->SetObjective(0.f);
		break;
	case LEVEL2:
		icons->GetChildren()[0]->GetComponent<UIApearDisapear>()->SetObjective(0.f);
		icons->GetChildren()[1]->GetComponent<UIApearDisapear>()->SetObjective(1.f);
		icons->GetChildren()[2]->GetComponent<UIApearDisapear>()->SetObjective(0.f);
		icons->GetChildren()[3]->GetComponent<UIApearDisapear>()->SetObjective(0.f);
		break;
	case LEVEL3:
		icons->GetChildren()[0]->GetComponent<UIApearDisapear>()->SetObjective(0.f);
		icons->GetChildren()[1]->GetComponent<UIApearDisapear>()->SetObjective(0.f);
		icons->GetChildren()[2]->GetComponent<UIApearDisapear>()->SetObjective(1.f);
		icons->GetChildren()[3]->GetComponent<UIApearDisapear>()->SetObjective(0.f);
		break;
	case LEVEL4:
		icons->GetChildren()[0]->GetComponent<UIApearDisapear>()->SetObjective(0.f);
		icons->GetChildren()[1]->GetComponent<UIApearDisapear>()->SetObjective(0.f);
		icons->GetChildren()[2]->GetComponent<UIApearDisapear>()->SetObjective(0.f);
		icons->GetChildren()[3]->GetComponent<UIApearDisapear>()->SetObjective(1.f);
		break;
	default:
		icons->GetChildren()[0]->GetComponent<UIApearDisapear>()->SetObjective(1.f);
		icons->GetChildren()[1]->GetComponent<UIApearDisapear>()->SetObjective(0.f);
		icons->GetChildren()[2]->GetComponent<UIApearDisapear>()->SetObjective(0.f);
		icons->GetChildren()[3]->GetComponent<UIApearDisapear>()->SetObjective(0.f);
		break;
	}
}

void UIEnemyBar::SetAppearNextCombat(bool nextCombat)
{
	appearNextCombat = nextCombat;
}