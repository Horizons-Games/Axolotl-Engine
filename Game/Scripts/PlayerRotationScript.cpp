#include "PlayerRotationScript.h"

#include "Application.h"
#include "ModuleInput.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentMeshCollider.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"

REGISTERCLASS(PlayerRotationScript);

PlayerRotationScript::PlayerRotationScript() : Script(), componentTransform(nullptr)
{
}

void PlayerRotationScript::Start()
{
	componentTransform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
}

void PlayerRotationScript::PreUpdate(float deltaTime)
{
	Rotate();
}

void PlayerRotationScript::Rotate()
{
	float deltaTime = App->GetDeltaTime();
	ComponentTransform* trans = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	float3 newRot = trans->GetRotationXYZ();
	ModuleInput* input = App->GetModule<ModuleInput>();
	newRot.y += -input->GetMouseMotion().x * deltaTime;
	trans->SetRotation(newRot);
	trans->UpdateTransformMatrices();


	//Corroborate that you don't fuse with a owner
	ComponentMeshCollider* collider = static_cast<ComponentMeshCollider*>(owner->GetComponent(ComponentType::MESHCOLLIDER));
	math::vec points[8];
	trans->GetObjectOBB().GetCornerPoints(points);
	std::vector<float3> frontPoints = { points[1], points[3], points[5], points[7] };
	float3 direction = (points[1] - points[0]).Normalized();
	if (collider->IsColliding(frontPoints, -direction, trans->GetLocalAABB().Size().z * 0.15f))
	{
		float deltaTime = App->GetDeltaTime();
		ComponentTransform* trans = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
		float3 newRot = trans->GetRotationXYZ();
		newRot.y += input->GetMouseMotion().x * deltaTime;
		trans->SetRotation(newRot);
		trans->UpdateTransformMatrices();
	}
}
