#include "StdAfx.h"
#include "Rotator.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"

REGISTERCLASS(Rotator);


Rotator::Rotator() : Script(), transform(nullptr), rotation(float3::zero),
velocity(0.0f), local(false)
{
	REGISTER_FIELD(rotation, float3);
	REGISTER_FIELD(velocity, float);
	REGISTER_FIELD(local, bool);
}

void Rotator::Start()
{
	transform = owner->GetComponent<ComponentTransform>();
}

void Rotator::Update(float deltaTime)
{
	rotation.Normalize();

	Quat n;
	float f = 1 / sqrtf(rotation.x * rotation.x + rotation.y * rotation.y + rotation.z * rotation.z);

	float angle = velocity * deltaTime;

	n.w = cosf(angle / 2);
	n.x = f * rotation.x * sinf(angle / 2);
	n.y = f * rotation.y * sinf(angle / 2);
	n.z = f * rotation.z * sinf(angle / 2);

	if (local) 
	{
		transform->SetLocalRotation(n * transform->GetLocalRotation());
		transform->CalculateMatrices();
		transform->UpdateTransformMatrices();
	}
	else 
	{
		transform->SetGlobalRotation(n * transform->GetGlobalRotation());
		transform->RecalculateLocalMatrix();
		transform->UpdateTransformMatrices();
	}
}