#pragma once

#include <string>
#include "Geometry/Frustum.h"

#include <memory>
#include <map>

#include "Geometry/Plane.h"
#include "Geometry/LineSegment.h"

enum EFrustumMode { normalFrustum, offsetFrustum, noFrustum };

enum class CameraType { UNKNOWN , C_ENGINE, C_GOD, C_GAMEOBJECT };

const static std::string GetNameByCameraType(CameraType type);
const static CameraType GetCameraTypeByName(const std::string& name);


class GameObject;
class WindowScene;

class Camera
{
public:
	Camera(const CameraType type);
	virtual ~Camera();

	virtual bool Init() = 0;
	virtual bool Start() = 0;

	virtual bool Update() = 0; // Abstract because each Camera will perform its own Update

	CameraType GetType();
	Frustum* GetFrustum();

	void ApplyRotation(const float3x3& rotationMatrix);

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
	void SetLookAt(const float3& lookAt);
	void SetMoveSpeed(float speed);
	void SetRotationSpeed(float speed);
	void SetFrustumOffset(float offset);
	void SetFrustumMode(int mode);
	void SetViewPlaneDistance(float distance);

	const float4x4& GetProjectionMatrix() const;
	const float4x4& GetViewMatrix() const;

	float GetHFOV() const;
	float GetVFOV() const;
	float GetZNear() const;
	float GetZFar() const;
	float GetMoveSpeed() const;
	float GetRotationSpeed() const;
	float GetDistance(const float3& point) const;
	float GetFrustumOffset() const;
	float GetViewPlaneDistance() const;
	int	GetFrustumMode() const;
	const float3& GetPosition() const;

protected:

	LineSegment CreateRaycastFromMousePosition(const WindowScene* windowScene);

	void CalculateHittedGameObjects(const LineSegment& ray);
	void SetNewSelectedGameObject(const std::map<float, GameObject*>& hittedGameObjects,
		const LineSegment& ray);

	CameraType type;
	Frustum* frustum;

	float3 position;

	float4x4 projectionMatrix;
	float4x4 viewMatrix;

	float aspectRatio;
	float acceleration;
	float moveSpeed;
	float rotationSpeed;
	float mouseSpeedModifier;
	float frustumOffset;
	float viewPlaneDistance;

	int frustumMode;

	math::Plane offsetFrustumPlanes[6];
};


inline Camera::Camera(const CameraType type)
	: type(type)
{
	frustum = new Frustum();
}

inline CameraType Camera::GetType()
{
	return this->type;
}

inline Frustum* Camera::GetFrustum()
{
	return this->frustum;
}

const std::string GetNameByCameraType(CameraType type)
{
	switch (type)
	{
	case CameraType::C_ENGINE:
		return "Camera_Engine";
		break;
	case CameraType::C_GOD:
		return "Camera_God";
		break;
	case CameraType::C_GAMEOBJECT:
		return "Camera_GameObject";
		break;
	default:
		assert(false && "Wrong camera type introduced");
		return "";
	}
}

const CameraType GetCameraTypeByName(const std::string& typeName)
{
	if (typeName == "Camera_Engine")
		return CameraType::C_ENGINE;
	if (typeName == "Camera_God")
		return CameraType::C_GOD;
	if (typeName == "Camera_GameObject")
		return CameraType::C_GAMEOBJECT;
	return CameraType::UNKNOWN;
}


inline const float3& Camera::GetPosition() const
{
	return position;
}

inline float Camera::GetViewPlaneDistance() const
{
	return viewPlaneDistance;
}

inline void Camera::SetViewPlaneDistance(float distance)
{
	viewPlaneDistance = distance;
	frustum->SetViewPlaneDistances(0.1f, distance);
}

