#include "StdAfx.h"

#include "UICursorScript.h"

#include "Application.h"
#include "ModuleUI.h"
#include "ModuleInput.h"

#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentTransform2D.h"
#include "Components/ComponentScript.h"


REGISTERCLASS(UICursorScript);

UICursorScript::UICursorScript() : Script(), cursor(nullptr)
{
	REGISTER_FIELD(cursor, GameObject*);
}

void UICursorScript::Start()
{
	cursorImage = cursor->GetComponent<ComponentImage>();
}

void UICursorScript::Update(float deltaTime)
{
	ModuleInput* input = App->GetModule<ModuleInput>();
	ModuleUI* ui = App->GetModule<ModuleUI>();
	
	float movementSpeed = 10.0f;
	float rotationSpeed = 6.0f;

	ComponentTransform2D* transform = cursorImage->GetOwner()->GetComponent<ComponentTransform2D>();

	float2 cursorPosition = ui->GetCursorPosition();
	float cursorRotation = ui->GetCursorRotation();
	
	if (input->GetDirection().horizontalMovement == JoystickHorizontalDirection::LEFT)
	{
		cursorPosition.x -= movementSpeed;

		if (input->GetDirection().verticalMovement == JoystickVerticalDirection::BACK)
		{
			if (cursorRotation < 111 || cursorRotation > 291)
			{
				cursorRotation += rotationSpeed;
			}
			else
			{
				cursorRotation -= rotationSpeed;
			}
		}
		else if (input->GetDirection().verticalMovement == JoystickVerticalDirection::FORWARD)
		{
			if (cursorRotation < 21 || cursorRotation > 201)
			{
				cursorRotation += rotationSpeed;
			}
			else
			{
				cursorRotation -= rotationSpeed;
			}
		}
		else
		{
			if (cursorRotation < 66 || cursorRotation > 246)
			{
				cursorRotation += rotationSpeed;
			}
			else
			{
				cursorRotation -= rotationSpeed;
			}
		}
	}
	else if (input->GetDirection().horizontalMovement == JoystickHorizontalDirection::RIGHT)
	{
		cursorPosition.x += movementSpeed;

		if (input->GetDirection().verticalMovement == JoystickVerticalDirection::FORWARD)
		{
			if (cursorRotation < 111 || cursorRotation > 291)
			{
				cursorRotation -= rotationSpeed;
			}
			else
			{
				cursorRotation += rotationSpeed;
			}
		}
		else if (input->GetDirection().verticalMovement == JoystickVerticalDirection::BACK)
		{
			if (cursorRotation < 21 || cursorRotation > 201)
			{
				cursorRotation -= rotationSpeed;
			}
			else
			{
				cursorRotation += rotationSpeed;
			}
		}
		else
		{
			if (cursorRotation < 66 || cursorRotation > 246)
			{
				cursorRotation -= rotationSpeed;
			}
			else
			{
				cursorRotation += rotationSpeed;
			}
		}
	}

	if (input->GetDirection().verticalMovement == JoystickVerticalDirection::FORWARD)
	{
		cursorPosition.y += movementSpeed;

		if (input->GetDirection().horizontalMovement == JoystickHorizontalDirection::NONE)
		{
			if (cursorRotation > 156 && cursorRotation < 336)
			{
				cursorRotation += rotationSpeed;
			}
			else
			{
				cursorRotation -= rotationSpeed;
			}
		}
	}
	else if (input->GetDirection().verticalMovement == JoystickVerticalDirection::BACK)
	{
		cursorPosition.y -= movementSpeed;

		if (input->GetDirection().horizontalMovement == JoystickHorizontalDirection::NONE)
		{
			if (cursorRotation > 156 && cursorRotation < 336)
			{
				cursorRotation -= rotationSpeed;
			}
			else
			{
				cursorRotation += rotationSpeed;
			}
		}
	}

	if (cursorRotation > 359) cursorRotation = 0;
	else if (cursorRotation < 0) cursorRotation = 359;

	ui->SetCursorPosition(cursorPosition);
	ui->SetCursorRotation(cursorRotation);

	transform->SetPosition(float3(cursorPosition.x, cursorPosition.y, 0));
	transform->SetRotation(float3(0, 0, cursorRotation));
	transform->CalculateMatrices();
}