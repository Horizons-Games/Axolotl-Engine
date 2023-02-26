#pragma once

#include "Camera.h"

class GameObject;

class CameraGod : public  Camera
{
	CameraGod();
	virtual ~CameraGod() override;

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

