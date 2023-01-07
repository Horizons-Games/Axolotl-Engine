#pragma once
#include "Component.h"

#include "Geometry/Frustum.h"
#include "Geometry/Plane.h"
#include "Geometry/OBB.h"

enum ECameraFrustumMode
{
	normalFrustum,
	offsetFrustum,
	noFrustum
};


class ComponentCamera : public Component
{
public:
	ComponentCamera(bool active, GameObject* owner);
	~ComponentCamera() override;

	void Update() override;

	void Draw();
	void UpdateFrustumOffset();
	bool IsInside(const OBB& obb);
	bool IsInsideOffset(const OBB& obb);

private:
	Frustum frustum;
	int frustumMode;
	float frustumOffset;
	Plane offsetFrustumPlanes[6];
	bool drawFrustum;
};

