#include "UIImageControl.h"

#include "Components/UI/ComponentTransform2D.h"
#include "ModuleInput.h"
#include "Components/UI/ComponentImage.h"
#include "AxoLog.h"

REGISTERCLASS(UIImageControl);

UIImageControl::UIImageControl() : Script(), trigger(nullptr), imageComponent(nullptr),
 imageTransform(nullptr)
{
	REGISTER_FIELD(trigger, GameObject*);
	REGISTER_FIELD(endPosition, float3);
}

void UIImageControl::Start()
{
	imageComponent = owner->GetComponent<ComponentImage>();
	imageTransform = owner->GetComponent<ComponentTransform2D>();
	startPosition = imageTransform->GetPosition();
	resultPositon = endPosition - startPosition;

}

void UIImageControl::Update(float deltaTime)
{
	resultPositon.Normalize();
	imageTransform->SetPosition(imageTransform->GetPosition() + resultPositon);
	imageTransform->CalculateMatrices();

}
