#include "CameraSample.h"

REGISTERCLASS(CameraSample);

CameraSample::CameraSample() : Script()
{
	transform = owner->GetComponent<ComponentTransform>();
	position = transform->GetGlobalPosition();
	REGISTER_FIELD(influenceRadius, float);
}

