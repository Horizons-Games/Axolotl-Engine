#include "StdAfx.h"
#include "UITextTrigger.h"

#include "ModulePlayer.h"
#include "Application.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentPlayer.h"

REGISTERCLASS(UITextTrigger);

UITextTrigger::UITextTrigger() : Script(), textBox(nullptr)
{
	REGISTER_FIELD(textBox, GameObject*);
}

UITextTrigger::~UITextTrigger()
{
}

void UITextTrigger::Start()
{
	player = App->GetModule<ModulePlayer>()->GetPlayer()->GetComponent<ComponentPlayer>();
	componentRigidBody = owner->GetComponent<ComponentRigidBody>();
}

void UITextTrigger::Update(float deltaTime)
{

}

void UITextTrigger::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->GetComponent<ComponentPlayer>())
	{
		if (!wasInside)
		{
			if (textBox != nullptr)
			{
				textBox->Enable();
			}
			wasInside = true;
		}
	}
}

void UITextTrigger::OnCollisionExit(ComponentRigidBody* other)
{
	if (other->GetOwner()->GetComponent<ComponentPlayer>())
	{
		if (wasInside)
		{
			if (textBox != nullptr)
			{
				textBox->Disable();
			}
			wasInside = true;
		}
	}
}
