#include "StdAfx.h"
#include "UITriggerEnemyBar.h"

#include "../Scripts/UIEnemyBar.h"

#include "ModulePlayer.h"
#include "Application.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "Components/UI/ComponentSlider.h"

REGISTERCLASS(UITriggerEnemyBar);

UITriggerEnemyBar::UITriggerEnemyBar() : Script(), enemyBar(nullptr)
{
	REGISTER_FIELD(enemyBar, UIEnemyBar*);
}


UITriggerEnemyBar::~UITriggerEnemyBar()
{
}

	
void UITriggerEnemyBar::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player"))
	{
		enemyBar->SetAppearNextCombat(true);
	}
}