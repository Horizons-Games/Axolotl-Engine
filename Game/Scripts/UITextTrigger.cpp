#include "StdAfx.h"
#include "UITextTrigger.h"

#include "UIMissionTrigger.h"
#include "UIEnemyBar.h"
#include "ModulePlayer.h"
#include "Application.h"
#include "ModuleInput.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentPlayer.h"

#include "PlayerManagerScript.h"
#include "UIImageDisplacementControl.h"

REGISTERCLASS(UITextTrigger);

UITextTrigger::UITextTrigger() : Script(), textBox{}, textBoxCurrent(0),
	textBoxSize(0), dialogueDone(false),
	mission(nullptr), finalBoss(false)
{
	REGISTER_FIELD(finalBoss, bool);
	REGISTER_FIELD(textBox, std::vector<GameObject*>);
	REGISTER_FIELD(mission, GameObject*);
}

UITextTrigger::~UITextTrigger()
{
}

void UITextTrigger::Start()
{
	componentRigidBody = owner->GetComponent<ComponentRigidBody>();
	input = App->GetModule<ModuleInput>();
	player = App->GetModule<ModulePlayer>()->GetPlayer();
	playerManager = player->GetComponent<PlayerManagerScript>();

	textBoxSize = static_cast<float>(textBox.size()) - 1.0f;
	displacementControl = textBox[static_cast<size_t>(textBoxCurrent)]->GetComponent<UIImageDisplacementControl>();
	currentText = textBox[static_cast<size_t>(textBoxCurrent)];
}

void UITextTrigger::Update(float deltaTime)
{
	if(player != App->GetModule<ModulePlayer>()->GetPlayer())
	{
		player = App->GetModule<ModulePlayer>()->GetPlayer();
		playerManager = player->GetComponent<PlayerManagerScript>();
	}

	if (dialogueDone)
	{
		return;
	}

	if(textBoxCurrent < textBoxSize)
	{
		if (wasInside && input->GetKey(SDL_SCANCODE_F) == KeyState::DOWN)
		{
			NextText();
			displacementControl = currentText->GetComponent<UIImageDisplacementControl>();

			currentText->Enable();
			displacementControl->SetImageToEndPosition();
		}
	}
	else if (wasInside && input->GetKey(SDL_SCANCODE_F) == KeyState::DOWN)
	{
		TextEnd();
	}
}

void UITextTrigger::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player") && !dialogueDone)
	{
		if (!wasInside)
		{
			if (currentText)
			{
				currentText->Enable();
				displacementControl->SetIsMoving(true);
				displacementControl->SetMovingToEnd(true);
				playerManager->PausePlayer(true);
			}
			wasInside = true;
		}
	}
}

void UITextTrigger::NextText()
{
	displacementControl->SetImageToStartPosition();
	currentText->Disable();

	textBoxCurrent = textBoxCurrent + 1;
	currentText = textBox[static_cast<size_t>(textBoxCurrent)];
}

void UITextTrigger::TextEnd()
{
	displacementControl->MoveImageToStartPosition();
	currentText->Disable();
	playerManager->PausePlayer(false);
	if (mission)
	{
		if (mission->HasComponent<UIMissionTrigger>())
		{
			mission->GetComponent<UIMissionTrigger>()->ActivateTextBoxManually();
		}
		else
		{
			mission->GetComponent<UIEnemyBar>()->SetAppearNextCombat(true);
		}
	}
	if (!finalBoss)
	{
		App->GetModule<ModulePlayer>()->SetInCombat(true);
	}
	else
	{
		App->GetModule<ModulePlayer>()->SetInBossCombat(true);
	}
	dialogueDone = true;
}