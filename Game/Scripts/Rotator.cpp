#include "StdAfx.h"
#include "Rotator.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"

REGISTERCLASS(Rotator);


Rotator::Rotator() : Script(), v(float3::zero), k(float3::zero), theta(180.0f), transform(nullptr), rotation(float3::zero),
velocity(0.0f)
{
	/*REGISTER_FIELD(v, float3);
	REGISTER_FIELD(k, float3);*/
	REGISTER_FIELD(rotation, float3);
	REGISTER_FIELD(velocity, float);
}

void Rotator::Start()
{
	transform = owner->GetComponent<ComponentTransform>();
}

void Rotator::Update(float deltaTime)
{
	/*double cos_theta = cos(theta);
	double sin_theta = sin(theta);

	float3 rotated = (v * cos_theta) + (math::Cross(k, v) * sin_theta) + (k * math::Dot(k, v)) * (1 - cos_theta);*/
	rotation.Normalize();

	Quat n;
	float f = 1 / sqrtf(rotation.x * rotation.x + rotation.y * rotation.y + rotation.z * rotation.z);

	float angle = velocity * deltaTime;

	n.w = cosf(angle / 2);
	n.x = f * rotation.x * sinf(angle / 2);
	n.y = f * rotation.y * sinf(angle / 2);
	n.z = f * rotation.z * sinf(angle / 2);

	transform->SetGlobalRotation(n * transform->GetGlobalRotation());
	transform->RecalculateLocalMatrix();
	transform->UpdateTransformMatrices();
}