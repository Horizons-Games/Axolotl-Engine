#include "Application.h"
#include "CameraSample.h"
#include "Components/Component.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"

REGISTERCLASS(CameraSample);

CameraSample::CameraSample() : Script(), 
	transform(nullptr),
	position(float3::zero),
	influenceRadius(1.0f),
	positionOffset(float3::zero),
	orientationOffset(float3::zero)
{
	REGISTER_FIELD(influenceRadius, float);
	REGISTER_FIELD(positionOffset, float3);
	REGISTER_FIELD(orientationOffset, float3);
}

void CameraSample::Start()
{
	transform = owner->GetComponent<ComponentTransform>();
	position = transform->GetGlobalPosition();
}



