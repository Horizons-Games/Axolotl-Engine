#include "UIImageDisplacementControl.h"

#include "Components/UI/ComponentTransform2D.h"
#include "Components/UI/ComponentImage.h"
#include "AxoLog.h"

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

}

void UIImageDisplacementControl::MoveImageToEndPosition()
{
	if (imageTransform->GetPosition().Distance(endPosition) >= 1.0f)
	{
		resultPositon = endPosition - startPosition;
		resultPositon.Normalize();
		imageTransform->SetPosition(imageTransform->GetPosition() + resultPositon);
		imageTransform->CalculateMatrices();
	}
}

void UIImageDisplacementControl::MoveImageToStarPosition()
{
	if (imageTransform->GetPosition().Distance(startPosition) >= 1.0f)
	{
		resultPositon = startPosition - endPosition;
		resultPositon.Normalize();
		imageTransform->SetPosition(imageTransform->GetPosition() + resultPositon);
		imageTransform->CalculateMatrices();
	}
}