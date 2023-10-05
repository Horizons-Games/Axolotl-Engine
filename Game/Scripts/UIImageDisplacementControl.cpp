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
	if (moving)
	{
		if (movingToEnd)
		{
			MoveImageToEndPosition();
		}
		else
		{
			MoveImageToStarPosition();
		}
	}

}

void UIImageDisplacementControl::MoveImageToEndPosition()
{
	if (imageTransform->GetPosition().Distance(endPosition) >=  10.0f)
	{
		moving = true;
		resultPositon = endPosition - startPosition;
		resultPositon.Normalize();
		imageTransform->SetPosition(imageTransform->GetPosition() + resultPositon * 15.0f);
		imageTransform->CalculateMatrices();
	}
	else
	{
		imageTransform->SetPosition(endPosition);
		imageTransform->CalculateMatrices();
		moving = false;
	}
}

void UIImageDisplacementControl::MoveImageToStarPosition()
{
	if (imageTransform->GetPosition().Distance(startPosition) >= 10.0f)
	{
		moving = true;
		resultPositon = startPosition - endPosition;
		resultPositon.Normalize();
		imageTransform->SetPosition(imageTransform->GetPosition() + resultPositon * 15.0f);
		imageTransform->CalculateMatrices();
	}
	else
	{
		imageTransform->SetPosition(startPosition);
		imageTransform->CalculateMatrices();
		moving = false;
	}
}