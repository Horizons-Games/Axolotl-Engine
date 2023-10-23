#include "StdAfx.h"
#include "UITextTrigger.h"

#include "UIMissionTrigger.h"
#include "ModulePlayer.h"
#include "Application.h"
#include "ModuleInput.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentPlayer.h"

#include "PauseManager.h"
#include "..\Game\Scripts\UIImageDisplacementControl.h"

REGISTERCLASS(UITextTrigger);

UITextTrigger::UITextTrigger() : Script(), textBox{}, textBoxCurrent(0),
	textBoxSize(0), pauseManager(nullptr), dialogueDone(false),
	mission(nullptr)
{
	REGISTER_FIELD(textBox, std::vector<GameObject*>);
	REGISTER_FIELD(pauseManager, GameObject*);
	REGISTER_FIELD(mission, GameObject*);
}

UITextTrigger::~UITextTrigger()
{
}

void UITextTrigger::Start()
{
	componentRigidBody = owner->GetComponent<ComponentRigidBody>();
	input = App->GetModule<ModuleInput>();

	textBoxSize = static_cast<float>(textBox.size()) - 1.0f;
	displacementControl = textBox[static_cast<size_t>(textBoxCurrent)]->GetComponent<UIImageDisplacementControl>();
	currentText = textBox[static_cast<size_t>(textBoxCurrent)];
}

void UITextTrigger::Update(float deltaTime)
{
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
				if(pauseManager->HasComponent<PauseManager>())
				{
					pauseManager->GetComponent<PauseManager>()->PausePlayer(true);
				}
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
	if (pauseManager->HasComponent<PauseManager>())
	{
		pauseManager->GetComponent<PauseManager>()->PausePlayer(false);
	}
	if (mission)
	{
		mission->GetComponent<UIMissionTrigger>()->ActivateTextBoxManually();
	}
	App->GetModule<ModulePlayer>()->SetInCombat(true);

	dialogueDone = true;
}