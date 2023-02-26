#pragma once

#include "Camera.h"

#define ORBIT_SPEED_MULTIPLIER 2.f

#define MAX_MOUSE_SPEED_MODIFIER 5.f
#define MAX_HFOV 120
#define MAX_VFOV 85
#define MAX_FRUSTUM 2.f

#define MIN_HFOV 60
#define MIN_VFOV 34
#define MIN_FRUSTUM -2.f

class GameObject;

class CameraEngine : public  Camera
{
public:
	CameraEngine();
	virtual ~CameraEngine() override;

	bool Update() override;

	void Move();
	void KeyboardRotate();
	void FreeLook();
	void UnlimitedCursor();
	void Run();
	void Walk();
	void Zoom();
	void Focus(const OBB& obb);
	void Focus(GameObject* gameObject);
	void Orbit(const OBB& obb);
	void Rotate();


private:
};