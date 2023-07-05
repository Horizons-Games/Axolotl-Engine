#pragma once
#include "Camera.h"

class CameraGameObject : public Camera
{
public:
	CameraGameObject();
	CameraGameObject(const std::unique_ptr<Camera>& camera);
	virtual ~CameraGameObject() override;

	bool Update() override;

	void SetPositionTarget(const float3& targetPosition, float deltaTime);
	void SetRotationTarget(const Quat& targetRotation, float deltaTime);

	void ApplyRotation(const Quat& rotationQuat);
	void ApplyRotationWithFixedUp(const Quat& rotationQuat, const float3& fixedUp);

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