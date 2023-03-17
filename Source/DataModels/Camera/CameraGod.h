#pragma once

#include "Camera.h"

class GameObject;

class CameraGod : public  Camera
{
public:
	CameraGod();
	CameraGod(const std::unique_ptr<Camera>& camera);
	virtual ~CameraGod() override;

	bool Update() override;

	void Move();

private:
	void keepMouseCentered();
};

