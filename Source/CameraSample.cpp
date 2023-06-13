#include "CameraSample.h"
#include "Components/ComponentTransform.h"

REGISTERCLASS(CameraSample);

CameraSample::CameraSample() : Script()
{
	transform = owner->GetComponent<ComponentTransform>();
	position = transform->GetGlobalPosition();
	REGISTER_FIELD(influenceRadius, float);
	positionOffset = position;
	orientationOffset = float3::zero;
}



