#include "ComponentTransform.h"
#include "GameObject/GameObject.h"

#include "EngineLog.h"

ComponentTransform::ComponentTransform(const bool active, GameObject* owner)
	: Component(ComponentType::TRANSFORM, active, owner), ownerParent(GetOwner()->GetParent())
{
}

ComponentTransform::~ComponentTransform()
{
	delete ownerParent;
}

void ComponentTransform::Update()
{
	CalculateLocalMatrix();
	CalculateGlobalMatrix();
}

void ComponentTransform::CalculateLocalMatrix()
{
	float4x4 localMatrix = float4x4::FromTRS((float3)GetPosition(), (Quat)GetRotation(), (float3)GetScale());

	SetLocalMatrix(localMatrix);
}

void ComponentTransform::CalculateGlobalMatrix()
{
	if (ownerParent != nullptr)
	{
		ComponentTransform* parentTransform = (ComponentTransform*)ownerParent->GetComponent(ComponentType::TRANSFORM);
		float4x4 globalMatrix = GetLocalMatrix() + parentTransform->GetGlobalMatrix();
		SetGlobalMatrix(globalMatrix);
	}
}