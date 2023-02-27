#pragma once

#include "Camera.h"

class GameObject;

class CameraEngine : public  Camera
{
public:
	CameraEngine();
	virtual ~CameraEngine() override;

	bool Update() override;

	void Move();
	void UnlimitedCursor();
	void Zoom();
	void Focus(const OBB& obb);
	void Focus(GameObject* gameObject);
	void Orbit(const OBB& obb);
	void Rotate();


private:
};