#pragma once

#include "Geometry/LineSegment.h"

struct RaycastHit 
{
	GameObject* gameObject;

	//TO DO: Split the GameObject information
};

class Physics
{
public:
	static bool ScreenPointToRay(const float2& mousePosition, LineSegment& ray);
	static bool Raycast(const LineSegment& ray, RaycastHit& hit);

private:

	void CalculateHitSelectedGo(std::map<float, const GameObject*>& hitGameObjects, const LineSegment& ray);
	void SetNewSelectedGameObject(const std::map<float, const GameObject*>& hitGameObjects, const LineSegment& ray);
};

