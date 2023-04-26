#include "ComponentMeshCollider.h"

#include "GameObject/GameObject.h"
#include "Application.h"

#include "FileSystem/Json.h"
#include "ComponentTransform.h"

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

bool ComponentMeshCollider::Move(Direction direction, float size, float stepSize )
{
	ComponentTransform* trans = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	float3 position = trans->GetPosition();

	math::vec points[8];
	trans->GetObjectOBB().GetCornerPoints(points);
	std::vector<float3> PositionPoints = { points[0], points[1],  points[2],  points[3],  points[4],  points[5],  points[6],  points[7]};
	std::vector<float3> usedPoints; 
	GetPointsGivenDirection(usedPoints, PositionPoints, direction);
	float3 movementDirection = GetMovementGivenDirection(PositionPoints, direction);

	if (!IsColliding(usedPoints, movementDirection, size, stepSize))
	{
		position += movementDirection * size;
		trans->SetPosition(position);

		trans->UpdateTransformMatrices();
		return true;
	}
	return false;
}

bool ComponentMeshCollider::StepsMove(int steps, Direction direction, float distance, RaycastHit& hit, float stepSize)
{
	float sectionMove = distance / (float)steps;

	ComponentTransform* trans = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	float3 position = trans->GetPosition();

	math::vec points[8];
	trans->GetObjectOBB().GetCornerPoints(points);
	std::vector<float3> PositionPoints = { points[0], points[1],  points[2],  points[3],  points[4],  points[5],  points[6],  points[7] };
	std::vector<float3> usedPoints;
	GetPointsGivenDirection(usedPoints, PositionPoints, direction);
	float3 movementDirection = GetMovementGivenDirection(PositionPoints, direction);
	for (int i = steps; i > 0; i--)
	{
		if(!IsColliding(usedPoints, movementDirection, sectionMove*(float)i, stepSize))
		{
			position += movementDirection * sectionMove * (float)i;
			trans->SetPosition(position);

			trans->UpdateTransformMatrices();
			return true;
		}
	}

	return false;
}

bool ComponentMeshCollider::IsColliding(std::vector<float3>& startingPoints, float3 direction, float size, float stepSize) const
{
	std::vector<float3> points;
	GetMinMaxPoints(startingPoints, points, stepSize);

	for (float3 point : points)
	{
		Ray ray(point, direction);
		LineSegment line(ray, size);
		bool hasHit = Physics::RaycastFirst(line);

		if (hasHit) {
			return true;
		}

	}

	return false;
}

void ComponentMeshCollider::GetMinMaxPoints(const std::vector<float3>& startingPoints, std::vector<float3>& points, float stepSize) const
{
	points = startingPoints;

	points.push_back(startingPoints[0] + (startingPoints[1] - startingPoints[0]) / 2);
	points.push_back(startingPoints[0] + (startingPoints[2] - startingPoints[0]) / 2);
	points.push_back(startingPoints[3] - (startingPoints[3] - startingPoints[2]) / 2);
	points.push_back(startingPoints[3] - (startingPoints[3] - startingPoints[1]) / 2);
	points.push_back(startingPoints[0] + (startingPoints[3] - startingPoints[0]) / 2);

	//Add stepSize to the top 3 down;
	if (stepSize != 0)
	{
		float minStep = points[0].y;
		for (float3 point : points)
		{
			if (minStep > point.y)
			{
				minStep = point.y;
			}
		}
		minStep += stepSize;
		for (int i = 0; i < points.size(); i++)
		{
			if (minStep > points[i].y)
			{
				points[i] = float3(points[i].x, minStep, points[i].z);
			}
		}
	}
}


void ComponentMeshCollider::GetPointsGivenDirection(std::vector<float3>& newPoints, std::vector<float3>& points, Direction direction) const
{
	switch (direction)
	{
	case Direction::FRONT:
		newPoints = { points[1], points[3], points[5], points[7] };
		break;
	case Direction::BACK:
		newPoints = { points[0], points[2], points[4], points[6] };
		break;
	case Direction::LEFT:
		newPoints = { points[4], points[6], points[5],  points[7] };
		break;
	case Direction::RIGHT:
		newPoints = { points[0], points[2], points[1], points[3] };
		break;
	case Direction::UP:
		newPoints = { points[0], points[1], points[4], points[5] };
		break;
	case Direction::DOWN:
		newPoints = { points[2], points[3], points[6], points[7] };
		break;
	}
}

float3 ComponentMeshCollider::GetMovementGivenDirection(std::vector<float3>& points, Direction direction)
{
	switch (direction)
	{
		case Direction::FRONT:
			return (points[1] - points[0]).Normalized();
		case Direction::BACK:
			return (points[0] - points[1]).Normalized();
		case Direction::LEFT:
			return (points[4] - points[0]).Normalized();
		case Direction::RIGHT:
			return (points[0] - points[4]).Normalized();
		case Direction::UP:
			return float3::unitY;
		case Direction::DOWN:
			return -float3::unitY;
		default:
			break;
	}
}