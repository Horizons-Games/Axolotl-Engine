#pragma once
#include "Component.h"

#include "Geometry/Frustum.h"
#include "Geometry/Plane.h"
#include "Geometry/OBB.h"
#include "ComponentTransform.h"

#define COMPONENT_CAMERA "Camera"
enum ECameraFrustumMode
{
	normalFrustum,
	offsetFrustum,
	noFrustum
};

class Json;

class ComponentCamera : public Component
{
public:
	ComponentCamera(bool active, GameObject* owner);
	~ComponentCamera() override;

	void Update() override;
	void Draw() override;
	void Display() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void UpdateFrustumOffset();
	bool IsInside(const OBB& obb);
	bool IsInsideOffset(const OBB& obb);

	void SetDrawFrustum(bool newFrustum);
	void SetFrustumMode(int newFrustumMode);
	void SetFrustumOffset(float newFrustumOffset);

	bool IsDrawFrustum() const;
	int GetFrustumMode() const;
	float GetFrustumOffset() const;


private:
	Frustum frustum;
	int frustumMode;
	float frustumOffset;
	Plane offsetFrustumPlanes[6];
	bool drawFrustum;
	ComponentTransform* trans;
};

inline void ComponentCamera::SetDrawFrustum(bool newFrustum)
{
	drawFrustum = newFrustum;
}

inline void ComponentCamera::SetFrustumMode(int newFrustumMode)
{
	frustumMode = newFrustumMode;
}

inline void ComponentCamera::SetFrustumOffset(float newFrustumOffset)
{
	frustumOffset = newFrustumOffset;
}

inline bool ComponentCamera::IsDrawFrustum() const
{
	return drawFrustum;
}

inline int ComponentCamera::GetFrustumMode() const
{
	return frustumMode;
}

inline float ComponentCamera::GetFrustumOffset() const
{
	return frustumOffset;
}