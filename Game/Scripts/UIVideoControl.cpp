#include "StdAfx.h"
#include "UIVideoControl.h"

#include "Components/UI/ComponentTransform2D.h"
#include "Components/UI/ComponentImage.h"

REGISTERCLASS(UIVideoControl);

UIVideoControl::UIVideoControl() : Script(), imageTransform(nullptr), optionMenuObject(nullptr)
{
	REGISTER_FIELD(optionMenuObject, GameObject*);
	REGISTER_FIELD(endPosition, float3);
}

void UIVideoControl::Start()
{
	imageTransform = owner->GetComponent<ComponentTransform2D>();
	startPosition = imageTransform->GetRotationXYZ();
}

void UIVideoControl::Update(float deltaTime)
{
	currentPositon = imageTransform->GetRotationXYZ();

	if (!optionMenuObject->IsEnabled() && currentPositon.y != endPosition.y)
	{
		imageTransform->SetRotation(endPosition);
		imageTransform->CalculateMatrices();
	}
	else if (optionMenuObject->IsEnabled() && currentPositon.y != startPosition.y)
	{
		imageTransform->SetRotation(startPosition);
		imageTransform->CalculateMatrices();
	}

}
