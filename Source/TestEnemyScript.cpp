#include "TestEnemyScript.h"

#include "Application.h"

#include "ModuleScene.h"
#include "ModuleCamera.h"

#include "Scene/Scene.h"
#include "DataStructures/Quadtree.h"

#include "Components/ComponentMeshCollider.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"

REGISTERCLASS(TestEnemyScript);

TestEnemyScript::TestEnemyScript() : Script()
{

}

void TestEnemyScript::PreUpdate(float deltaTime)
{

	ComponentTransform* trans = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	ComponentMeshCollider* collider = static_cast<ComponentMeshCollider*>(owner->GetComponent(ComponentType::MESHCOLLIDER));
	ComponentRigidBody* rigidBody = static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));

	math::vec points[8];
	trans->GetObjectOBB().GetCornerPoints(points);
	std::vector<float3> bottomPoints = { points[0], points[1], points[4], points[5] };

	RaycastHit hit;

	trans->UpdateTransformMatrices();

	//bottom
	float maxHeight = -math::inf;

	std::vector<float3> extraPoints;
	collider->GetMinMaxPoints(bottomPoints, extraPoints, 0);
	for (float3 bottomPoint : extraPoints)
	{
		bottomPoint.y += math::Abs(trans->GetEncapsuledAABB().MinY() - trans->GetPosition().y) / 10;
		Ray ray(bottomPoint, -float3::unitY);
		LineSegment line(ray, App->scene->GetLoadedScene()->GetRootQuadtree()->GetBoundingBox().Size().y);
		bool hasHit = Physics::Raycast(line, hit, owner);

		if (hasHit && hit.hitPoint.y > maxHeight)
		{
			maxHeight = hit.hitPoint.y;
		}

	}

	rigidBody->SetBottomHitPoint(maxHeight);
}