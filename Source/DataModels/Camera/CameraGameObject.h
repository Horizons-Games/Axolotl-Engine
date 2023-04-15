#pragma once
#include "Camera.h"

class CameraGameObject : public Camera
{
public:
	CameraGameObject();
	CameraGameObject(const std::unique_ptr<Camera>& camera);
	virtual ~CameraGameObject() override;

	bool Update() override;

	void UpdateCameraWithMousePos();

	bool IsDrawFrustum();
	void SetIsDrawFrustum(bool newIsDrawFrustum);

private:
	bool isDrawFrustum;
};

inline bool CameraGameObject::IsDrawFrustum()
{
	return this->isDrawFrustum;
}

inline void CameraGameObject::SetIsDrawFrustum(bool newIsDrawFrustum)
{
	isDrawFrustum = newIsDrawFrustum;
}