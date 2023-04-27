#pragma once

#include <string>
#include <memory>
#include "Geometry/Frustum.h"
#include <map>

#include "Geometry/Plane.h"
#include "Geometry/Frustum.h"
#include "Math/float4x4.h"
#include "Math/Quat.h"

enum class EFrustumMode 
{ 
	normalFrustum, 
	offsetFrustum, 
	noFrustum 
};

#define DEFAULT_MOVE_SPEED 9.f
#define DEFAULT_ROTATION_DEGREE 30
#define DEFAULT_ROTATION_SPEED 5.f
#define DEFAULT_MOUSE_SPEED_MODIFIER 0.f
#define DEFAULT_MOUSE_ZOOM_SPEED 2.f
#define DEFAULT_SHIFT_ACCELERATION 2.f
#define DEFAULT_FRUSTUM_MODE EFrustumMode::normalFrustum
#define DEFAULT_FRUSTUM_OFFSET 1.f
#define DEFAULT_FRUSTUM_DISTANCE 20000.f
#define DEFAULT_GAMEOBJECT_FRUSTUM_DISTANCE 2000.f
enum class CameraType 
{ 
	C_ENGINE, 
	C_GOD, 
	C_GAMEOBJECT 
};

class GameObject;
class WindowScene;
struct RaycastHit;

class Camera
{
public:
	Camera(const CameraType type);
	Camera(Camera& camera);
	Camera(const std::unique_ptr<Camera>& camera,const CameraType type);
	virtual ~Camera();

	virtual bool Init();
	virtual bool Start();

	virtual bool Update() = 0; // Abstract because each Camera will perform its own Update

	CameraType GetType();
	Frustum* GetFrustum();

	void ApplyRotation(const float3x3& rotationMatrix);
	void ApplyRotation(const Quat& rotationQuat);
	void ApplyRotationWithFixedUp(const float3x3& rotationMatrix, const float3& fixedUp);
	void ApplyRotationWithFixedUp(const Quat& rotationQuat, const float3& fixedUp);
	
	void Run();
	void Walk();
	void KeyboardRotate();
	void FreeLook();


	bool IsInside(const OBB& obb);
	bool IsInside(const AABB& aabb);
	bool IsInsideOffset(const OBB& obb);
	void RecalculateOffsetPlanes();

	void SetHFOV(float fov);
	void SetVFOV(float fov);
	void SetAspectRatio(float aspect);
	void SetPlaneDistance(float zNear, float zFar);
	void SetPosition(const float3& position);
	void SetOrientation(const float3& orientation);
	void SetMoveSpeed(float speed);
	void SetRotationSpeed(float speed);
	void SetFrustumOffset(float offset);
	void SetFrustumMode(EFrustumMode mode);
	void SetViewPlaneDistance(float distance);
	void SetKpPosition(float KpPosition);
	void SetKpRotation(float KpRotation);
	void SetInterpolationDuration(float interpolationDuration);
	void SetFocusing(bool isFocusing);
	void SetUsingProportionalController(bool isUsingProportionalController);

	const float4x4& GetProjectionMatrix() const;
	const float4x4& GetViewMatrix() const;

	float GetHFOV() const;
	float GetVFOV() const;
	float GetAspectRatio() const;
	float GetZNear() const;
	float GetZFar() const;
	float GetMoveSpeed() const;
	float GetRotationSpeed() const;
	float GetDistance(const float3& point) const;
	float GetFrustumOffset() const;
	float GetViewPlaneDistance() const;
	bool IsFocusing() const;
	bool IsUsingProportionalController() const;
	float GetKpPosition() const;
	float GetKpRotation() const;
	float GetInterpolationDuration() const;
	EFrustumMode GetFrustumMode() const;
	const float3& GetPosition() const;
	const Quat& GetRotation() const;

protected:
	
	void SetNewSelectedGameObject(GameObject* gameObject);

	CameraType type;
	std::unique_ptr<Frustum> frustum;

	float3 position;
	Quat rotation;
	float4x4 projectionMatrix;
	float4x4 viewMatrix;
	float aspectRatio;
	float acceleration;
	float moveSpeed;
	float rotationSpeed;
	float mouseSpeedModifier;
	float frustumOffset;
	float viewPlaneDistance;

	EFrustumMode frustumMode;
	math::Plane offsetFrustumPlanes[6];

	bool isFocusing;
	bool isUsingProportionalController;
	float KpPosition;
	float KpRotation;
	float interpolationTime;
	float interpolationDuration;

	bool mouseWarped;
	int lastMouseX, lastMouseY;
	int mouseState;
};

inline CameraType Camera::GetType()
{
	return this->type;
}

inline Frustum* Camera::GetFrustum()
{
	return frustum.get();
}

inline const float3& Camera::GetPosition() const
{
	return position;
}

inline const Quat& Camera::GetRotation() const
{
	return rotation;
}

inline float Camera::GetKpPosition() const
{
	return KpPosition;
}

inline float Camera::GetKpRotation() const
{
	return KpRotation;
}

inline float Camera::GetInterpolationDuration() const
{
	return interpolationDuration;
}

inline float Camera::GetViewPlaneDistance() const
{
	return viewPlaneDistance;
}

inline bool Camera::IsFocusing() const
{
	return isFocusing;
}

inline bool Camera::IsUsingProportionalController() const
{
	return isUsingProportionalController;
}

inline void Camera::SetMoveSpeed(float speed)
{
	moveSpeed = speed;
}

inline void Camera::SetRotationSpeed(float speed)
{
	rotationSpeed = speed;
}

inline void Camera::SetFrustumOffset(float offset)
{
	frustumOffset = offset;
}

inline void Camera::SetFrustumMode(EFrustumMode mode)
{
	frustumMode = mode;
}

inline void Camera::SetViewPlaneDistance(float distance)
{
	viewPlaneDistance = distance;
	frustum->SetViewPlaneDistances(0.1f, distance);
}

inline void Camera::SetKpPosition(float Kp)
{
	KpPosition = Kp;
}

inline void Camera::SetKpRotation(float Kp)
{
	KpRotation = Kp;
}

inline void Camera::SetInterpolationDuration(float duration)
{
	interpolationDuration = duration;
}

inline void Camera::SetFocusing(bool focusing)
{
	isFocusing = focusing;
}

inline void Camera::SetUsingProportionalController(bool focusController)
{
	isUsingProportionalController = focusController;
}

inline const float4x4& Camera::GetProjectionMatrix() const
{
	return projectionMatrix;
}

inline const float4x4& Camera::GetViewMatrix() const
{
	return viewMatrix;
}

inline float Camera::GetHFOV() const
{
	return math::RadToDeg(frustum->HorizontalFov());
}

inline float Camera::GetVFOV() const
{
	return math::RadToDeg(frustum->VerticalFov());
}

inline float Camera::GetAspectRatio() const
{
	return aspectRatio;
}

inline float Camera::GetZNear() const
{
	return frustum->NearPlaneDistance();
}

inline float Camera::GetZFar() const
{
	return frustum->FarPlaneDistance();
}

inline float Camera::GetMoveSpeed() const
{
	return moveSpeed;
}

inline float Camera::GetRotationSpeed() const
{
	return rotationSpeed;
}

inline float Camera::GetDistance(const float3& point) const
{
	return frustum->Pos().Distance(point);
}

inline float Camera::GetFrustumOffset() const
{
	return frustumOffset;
}

inline EFrustumMode Camera::GetFrustumMode() const
{
	return frustumMode;
}