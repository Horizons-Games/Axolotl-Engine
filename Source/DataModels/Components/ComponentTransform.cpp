#include "ComponentTransform.h"
#include "GameObject/GameObject.h"

ComponentTransform::ComponentTransform(const bool active, GameObject* owner)
	: Component(ComponentType::TRANSFORM, active, owner)
{
	ownerParent = this->GetOwner()->GetParent();
}

ComponentTransform::~ComponentTransform()
{
	delete ownerParent;
}

void ComponentTransform::Update()
{
	// TODO: Perform Transform updates (move, rotate, scale...)
	CalculateLocalMatrix();
	CalculateGlobalMatrix();
}
void ComponentTransform::CalculateLocalMatrix()
{
	float3 position = GetPosition();
	float3 rotation = GetRotation();
	float3 scale = GetScale();

	float3x3 localMatrix = {
		position.x, position.y, position.z,
		rotation.x, rotation.y, rotation.z,
		scale.x, scale.y, scale.z
	};

	SetLocalMatrix(localMatrix);
	
}

void ComponentTransform::CalculateGlobalMatrix()
{
	if (ownerParent != nullptr)
	{
		ComponentTransform* parentTransform = (ComponentTransform*)ownerParent->GetComponent(ComponentType::TRANSFORM);
		float3x3 globalMatrix = GetLocalMatrix() + parentTransform->GetGlobalMatrix();
		SetGlobalMatrix(globalMatrix);
	}
}