#pragma once

#include "Camera.h"

class GameObject;

class CameraGod : public  Camera
{
public:
	CameraGod();
	virtual ~CameraGod() override;

	bool Update() override;

	void Move();

private:

};

