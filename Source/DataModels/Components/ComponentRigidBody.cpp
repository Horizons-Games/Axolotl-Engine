#include "ComponentRigidBody.h"
#include "ComponentTransform.h"

#include "GameObject/GameObject.h"
#include "Application.h"

#include "FileSystem/Json.h"

#include "Geometry/LineSegment.h"
#include "Geometry/Ray.h"
#include "Physics/Physics.h"

ComponentRigidBody::ComponentRigidBody(bool active, GameObject* owner)
	: Component(ComponentType::RIGIDBODY, active, owner, true)
{
	isKinematic = true;
	m = 1.0f;
	g = 9.81;
	v0 = float3(0.0f, 0.0f, 0.0f);
}

ComponentRigidBody::~ComponentRigidBody()
{
}

void ComponentRigidBody::Update()
{
#ifndef ENGINE
	bool stopped = false;
	if (isKinematic)
	{
		ComponentTransform* transform = static_cast<ComponentTransform*>(GetOwner()->GetComponent(ComponentType::TRANSFORM));
		float3 currentPos = transform->GetPosition();
		Ray ray(currentPos, -float3::unitY);
		LineSegment line(ray, 100.0f);
		RaycastHit hit;
		if (!stopped) 
		{
			Physics::Raycast(line, hit);
			float3 x;
			float t = App->GetDeltaTime();
			float3 x0 = currentPos;
			float3 a = float3(0.0f, -0.5 * g * t * t * 15, 0.0f);

			x = x0 + v0 * t + a;

			if (hit.gameObject == nullptr || x.y > hit.hitPoint.y + 3.0f)
			{
				transform->SetPosition(x);
			}
			else 
			{
				x = hit.hitPoint;
				transform->SetPosition(x);
				stopped = true;
				
				if (hit.gameObject->CompareTag("WIN"))
				{
				}
			}
		}
	}
#endif
}

void ComponentRigidBody::Draw()
{

}

void ComponentRigidBody::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;
	meta["isKinematic"] = (bool)GetIsKinematic();
}

void ComponentRigidBody::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];
	SetIsKinematic((bool)meta["isKinematic"]);
}
