#include "ComponentMeshCollider.h"

#include "GameObject/GameObject.h"
#include "Application.h"

#include "FileSystem/Json.h"

#include "Geometry/LineSegment.h"
#include "Geometry/Ray.h"
#include "Physics/Physics.h"

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

bool ComponentMeshCollider::IsColliding(std::vector<float3>& startingPoints, float3 direction, float size) const
{
	std::vector<float3> points;
	GetMinMaxPoints(startingPoints, points);

	for (float3 point : points)
	{
		Ray ray(point, direction);
		LineSegment line(ray, size);
		RaycastHit hit;
		bool hasHit = Physics::Raycast(line, hit);

		if (hasHit) {
			return true;
		}

	}

	return false;
}

void ComponentMeshCollider::GetMinMaxPoints(std::vector<float3>& startingPoints, std::vector<float3>& points) const
{
	points = startingPoints;

	points.push_back(startingPoints[0] + (startingPoints[1] - startingPoints[0]) / 2);
	points.push_back(startingPoints[0] + (startingPoints[2] - startingPoints[0]) / 2);
	points.push_back(startingPoints[1] + (startingPoints[2] - startingPoints[1]) / 2);
	points.push_back(startingPoints[3] - (startingPoints[3] - startingPoints[2]) / 2);
	points.push_back(startingPoints[0] + (startingPoints[3] - startingPoints[0]) / 2);
}