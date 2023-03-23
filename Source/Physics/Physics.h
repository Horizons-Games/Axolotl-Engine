#pragma once

#include "Geometry/LineSegment.h"

struct RaycastHit 
{

};

class Physics
{
public:
	static const LineSegment ScreenPointToRay(float2 mousePostion);
	static bool Raycast(LineSegment ray, RaycastHit& hit);

private:

};

