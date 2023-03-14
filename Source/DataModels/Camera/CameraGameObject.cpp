#include "CameraGameObject.h"


CameraGameObject::CameraGameObject()
	: Camera(CameraType::C_GAMEOBJECT)
{
};

CameraGameObject::CameraGameObject(const std::unique_ptr<Camera>& camera)
	: Camera(camera, CameraType::C_GAMEOBJECT)
{
}

CameraGameObject::~CameraGameObject()
{
};


bool CameraGameObject::Update()
{
	projectionMatrix = frustum->ProjectionMatrix();
	viewMatrix = frustum->ViewMatrix();
	return true;
}