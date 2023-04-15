#include "ComponentMeshCollider.h"

#include "GameObject/GameObject.h"
#include "Application.h"

#include "FileSystem/Json.h"

#include "Geometry/LineSegment.h"
#include "Geometry/Ray.h"
#include "Physics/Physics.h"

#include <iostream>

ComponentMeshCollider::ComponentMeshCollider(bool active, GameObject* owner)
	: Component(ComponentType::MESHCOLLIDER, active, owner, true),
	isTrigger(false)
{
}

ComponentMeshCollider::~ComponentMeshCollider()
{
}

void ComponentMeshCollider::Update()
{

}

void ComponentMeshCollider::Draw()
{

}

void ComponentMeshCollider::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;
}

void ComponentMeshCollider::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];
}
