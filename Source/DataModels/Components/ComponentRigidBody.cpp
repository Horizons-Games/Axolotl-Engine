#include "ComponentRigidBody.h"
#include "ComponentTransform.h"

#include "GameObject/GameObject.h"
#include "Application.h"

#include "FileSystem/Json.h"

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

	if (isKinematic)
	{
		ComponentTransform* transform = static_cast<ComponentTransform*>(GetOwner()->GetComponent(ComponentType::TRANSFORM));
		float3 x;
		float t = App->GetDeltaTime();
		float3 x0 = transform->GetPosition();
		float3 a = float3(0.0f, -0.5 * g * t * t*5, 0.0f);

		x = x0 + v0 * t + a;

		transform->SetPosition(x);
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
}

void ComponentRigidBody::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];
}
