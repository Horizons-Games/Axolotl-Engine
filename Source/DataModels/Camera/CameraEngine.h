#pragma once

#include "Camera.h"
#include "Math/float3.h"
class GameObject;

class CameraEngine : public Camera
{
public:
	CameraEngine();
	CameraEngine(const std::unique_ptr<Camera>& camera);
	virtual ~CameraEngine() override;

	bool Update() override;

	void Move();
	void UnlimitedCursor();
	void Zoom();
	void FocusProportional(const OBB& obb);
	void FocusInterpolated(const OBB& obb);
	void Focus(GameObject* gameObject);
	void Orbit(const OBB& obb);

private:

	float3 currentFocusPos;
	float3 currentFocusDir;

};