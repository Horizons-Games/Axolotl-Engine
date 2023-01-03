#pragma once
#include "Component.h"

#include "Geometry/Frustum.h"
#include "Geometry/Plane.h"

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

private:
	Frustum frustum;
	int frustumMode;
	float frustumOffset;
	Plane offsetFrustumPlanes[6];
	bool drawFrustum;
};

