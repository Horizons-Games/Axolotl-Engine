#pragma once

#include "Geometry/LineSegment.h"

#include <map>

class GameObject;
class Quadtree;

struct RaycastHit 
{
	GameObject* gameObject;
	float distance;
	float3 hitPoint;
	float3 normal;

	//TO DO: Obtain Collider/AABB from GameObject
};

class Physics
{
public:
	static bool ScreenPointToRay(const float2& mousePosition, LineSegment& ray);
	static float2 ScreenToScenePosition(const float2& mousePosition);
	static bool Raycast(const LineSegment& ray, RaycastHit& hit);
	static bool Raycast(const LineSegment& ray, RaycastHit& hit, GameObject* exceptionGameObject);
	static bool RaycastFirst(const LineSegment& ray);
	static bool RaycastFirst(const LineSegment& ray, GameObject* exceptionGameObject);
	static bool HasIntersection(const LineSegment& ray, GameObject* go, float& nearDistance, float& farDistance);

private:

	static void AddIntersectionGameObject(std::map<float, const GameObject*>& hitGameObjects,
									    const LineSegment& ray, GameObject* go);

	static void AddIntersectionQuadtree(std::map<float, const GameObject*>& hitGameObjects,
										const LineSegment& ray, Quadtree* quadtree);
	
	static void AddIntersectionDynamicObjects(std::map<float, const GameObject*>& hitGameObjects,
										const LineSegment& ray, const std::vector<GameObject*>& dynamicObjects);

	static void AddFirstFoundIntersectionQuadtree(std::map<float, const GameObject*>& hitGameObjects,
		const LineSegment& ray, Quadtree* quadtree);

	static void AddFirstFoundIntersectionDynamicObjects(std::map<float, const GameObject*>& hitGameObjects,
		const LineSegment& ray, const std::vector<GameObject*>& dynamicObjects);

	static void GetRaycastHitInfo(const std::map<float, const GameObject*>& hitGameObjects,
										const LineSegment& ray, RaycastHit& hit);

	static void GetRaycastHitInfo(const std::map<float, const GameObject*>& hitGameObjects,
		const LineSegment& ray, RaycastHit& hit, GameObject* exceptionGameObject);
};

