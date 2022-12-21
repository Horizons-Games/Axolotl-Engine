#pragma once

#include "Module.h"
#include "ModuleDebugDraw.h"

#include "Geometry/Frustum.h"
#include "Math/float4x4.h"

#define DEFAULT_MOVE_SPEED 5.f
#define DEFAULT_ROTATION_DEGREE 25
#define DEFAULT_ROTATION_SPEED 5.f
#define DEFAULT_MOUSE_SPEED_MODIFIER 10.f
#define DEFAULT_SHIFT_ACCELERATION 2.f

#define ORBIT_SPEED_MULTIPLIER 2.f

#define MAX_MOUSE_SPEED_MODIFIER 50.f
#define MAX_HFOV 120
#define MAX_VFOV 85

#define MIN_HFOV 60
#define MIN_VFOV 34

class ModuleEngineCamera : public Module
{
public:
	ModuleEngineCamera();
	~ModuleEngineCamera() override;

	bool Init() override;
	bool Start() override;

	update_status Update();

	void Move();
	void KeyboardRotate();
	void ApplyRotation(const float3x3& rotationMatrix);
	void FreeLook();
	void Run();
	void Walk();
	void Zoom();
	void Focus(const OBB& obb);
	void Orbit(const OBB& obb);

	void SetHFOV(float fov);
	void SetVFOV(float fov);
	void SetAspectRatio(float aspect);
	void SetPlaneDistance(float zNear, float zFar);
	void SetPosition(const float3& position);
	void SetOrientation(const float3& orientation);
	void SetLookAt(const float3& lookAt);
	void SetMoveSpeed(float speed);
	void SetRotationSpeed(float speed);

	const float4x4& GetProjectionMatrix() const;
	const float4x4& GetViewMatrix() const;

	float GetHFOV() const;
	float GetVFOV() const;
	float GetZNear() const;
	float GetZFar() const;
	float GetMoveSpeed() const;
	float GetRotationSpeed() const;
	float GetDistance(const float3& point) const;

private:
	Frustum frustum;
	float3 position;
	float4x4 projectionMatrix;
	float4x4 viewMatrix;
	float aspectRatio;
	float acceleration;
	float moveSpeed;
	float rotationSpeed;
	float mouseSpeedModifier;
};

