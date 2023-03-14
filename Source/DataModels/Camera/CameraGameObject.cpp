#include "CameraGameObject.h"


CameraGameObject::CameraGameObject()
	: Camera(CameraType::C_ENGINE)
{
};

CameraGameObject::CameraGameObject(const std::unique_ptr<Camera>& camera)
	: Camera(camera, CameraType::C_ENGINE)
{
}

CameraGameObject::~CameraGameObject()
{
};


bool CameraGameObject::Update()
{
	return true;
}