#include "StdAfx.h"
#include "UITextTrigger.h"

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
	textBoxSize(0), pauseManager(nullptr), dialogueDone(false)
{
	REGISTER_FIELD(textBox, std::vector<GameObject*>);
	REGISTER_FIELD(pauseManager, GameObject*);
}

UITextTrigger::~UITextTrigger()
{
}

void UITextTrigger::Start()
{
	componentRigidBody = owner->GetComponent<ComponentRigidBody>();
	input = App->GetModule<ModuleInput>();

	textBoxSize = textBox.size() - 1;
	displacementControl = textBox[textBoxCurrent]->GetComponent<UIImageDisplacementControl>();
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
			displacementControl = textBox[textBoxCurrent]->GetComponent<UIImageDisplacementControl>();

			textBox[textBoxCurrent]->Enable();
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
			if (textBox[textBoxCurrent] != nullptr)
			{
				textBox[textBoxCurrent]->Enable();
				displacementControl->SetIsMoving(true);
				displacementControl->SetMovingToEnd(true);
				if(pauseManager->HasComponent<PauseManager>())
				{
					pauseManager->GetComponent<PauseManager>()->Pause(true);
				}
			}
			wasInside = true;
		}
	}
}

void UITextTrigger::NextText()
{
	displacementControl->SetImageToStartPosition();
	textBox[textBoxCurrent]->Disable();

	textBoxCurrent = textBoxCurrent + 1;
}

void UITextTrigger::TextEnd()
{
	displacementControl->MoveImageToStartPosition();
	textBox[textBoxCurrent]->Disable();
	if (pauseManager->HasComponent<PauseManager>())
	{
		pauseManager->GetComponent<PauseManager>()->Pause(false);
	}

	dialogueDone = true;
}