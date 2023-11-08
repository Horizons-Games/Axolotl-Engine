#include "UIImageDisplacementControl.h"

#include "Components/UI/ComponentTransform2D.h"
#include "Components/UI/ComponentImage.h"

REGISTERCLASS(UIImageDisplacementControl);

UIImageDisplacementControl::UIImageDisplacementControl() : Script(), imageTransform(nullptr)
{
	REGISTER_FIELD(endPosition, float3);
}

void UIImageDisplacementControl::Start()
{
	imageTransform = owner->GetComponent<ComponentTransform2D>();
	startPosition = imageTransform->GetPosition();
}

void UIImageDisplacementControl::Update(float deltaTime)
{
	if (isMoving)
	{
		if (isMovingToEnd)
		{
			MoveImageToEndPosition();
		}
		else
		{
			MoveImageToStartPosition();
		}
	}

}

void UIImageDisplacementControl::MoveImageToEndPosition()
{
	if (imageTransform)
	{
		if (imageTransform->GetPosition().Distance(endPosition) >=  10.0f)
		{
			isMoving = true;
			resultPositon = endPosition - startPosition;
			resultPositon.Normalize();
			imageTransform->SetPosition(imageTransform->GetPosition() + resultPositon * 15.0f);
			imageTransform->CalculateMatrices();
		}
		else
		{
			imageTransform->SetPosition(endPosition);
			imageTransform->CalculateMatrices();
			isMoving = false;
		}
	}
}

void UIImageDisplacementControl::SetImageToEndPosition()
{
	imageTransform->SetPosition(endPosition);
	imageTransform->CalculateMatrices();
	isMoving = false;
}

void UIImageDisplacementControl::MoveImageToStartPosition()
{
	if (imageTransform)
	{
		if (imageTransform->GetPosition().Distance(startPosition) >= 10.0f)
		{
			isMoving = true;
			resultPositon = startPosition - endPosition;
			resultPositon.Normalize();
			imageTransform->SetPosition(imageTransform->GetPosition() + resultPositon * 15.0f);
			imageTransform->CalculateMatrices();
		}
		else
		{
			imageTransform->SetPosition(startPosition);
			imageTransform->CalculateMatrices();
			isMoving = false;
		}

	}
}

void UIImageDisplacementControl::SetImageToStartPosition()
{
	imageTransform->SetPosition(startPosition);
	imageTransform->CalculateMatrices();
	isMoving = false;
}

bool UIImageDisplacementControl::IsImageInStartPosition() const
{
	if (imageTransform->GetPosition().x == startPosition.x && 
		imageTransform->GetPosition().y == startPosition.y && 
		imageTransform->GetPosition().z == startPosition.z)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool UIImageDisplacementControl::IsMovingToEnd() const
{
	return isMovingToEnd;
}

void UIImageDisplacementControl::SetMovingToEnd(bool movingToEnd)
{
	this->isMovingToEnd = movingToEnd;
}

bool UIImageDisplacementControl::IsMoving() const
{
	return isMoving;
}

void UIImageDisplacementControl::SetIsMoving(bool moving)
{
	this->isMoving = moving;
}

void UIImageDisplacementControl::SetEndPosition(float3 endPosition)
{
	this->endPosition = endPosition;
}