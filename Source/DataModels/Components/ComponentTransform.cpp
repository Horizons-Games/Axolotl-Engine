#include "ComponentTransform.h"
#include "GameObject/GameObject.h"

#include "EngineLog.h"

#include "Math/float3x3.h"

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
	assert(ownerParent != nullptr);

	float3 parentPos, parentSca, localPos, localSca;
	Quat parentRot, localRot;

	ComponentTransform* parentTransform = (ComponentTransform*)ownerParent->GetComponent(ComponentType::TRANSFORM);

	parentTransform->GetGlobalMatrix().Decompose(parentPos, parentRot, parentSca);
	GetLocalMatrix().Decompose(localPos, localRot, localSca);

	float3 position = localPos + parentPos;
	Quat rotation = localRot * parentRot;
	float3 scale = parentSca.Mul(localSca);

	float4x4 globalMatrix = float4x4::FromTRS(position, rotation, scale);
	SetGlobalMatrix(globalMatrix);
}