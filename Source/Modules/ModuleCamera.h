#pragma once
#include "Module.h"
#include "ModuleDebugDraw.h"

#define ORBIT_SPEED_MULTIPLIER 2.f

#define MAX_MOUSE_SPEED_MODIFIER 5.f
#define MAX_HFOV 120
#define MAX_VFOV 85
#define MAX_FRUSTUM 2.f

#define MIN_HFOV 60
#define MIN_VFOV 34
#define MIN_FRUSTUM -2.f

class GameObject;
class WindowScene;
class Camera;

enum class CameraType;

class ModuleCamera : public Module
{
public:
	ModuleCamera();
	~ModuleCamera() override;

	bool Init() override;
	bool Start() override;

	UpdateStatus Update();

	Camera* GetCamera();
	void ChangeCamera(CameraType newType);
	Camera* GetSelectedCamera() const;
	void SetSelectedCamera(int cameraNumber);
	int GetSelectedPosition();
	void SetSelectedPosition(int newSelected);

	float4x4 GetOrthoProjectionMatrix() const;
	void RecalculateOrthoProjectionMatrix();

private:
	std::unique_ptr<Camera> camera;
	Camera* selectedCamera;
	int selectedPosition;

	float4x4 orthoProjectionMatrix;
};

inline Camera* ModuleCamera::GetCamera()
{
	return selectedCamera;
}

inline Camera* ModuleCamera::GetSelectedCamera() const
{
	return selectedCamera;
}

inline int ModuleCamera::GetSelectedPosition()
{
	return selectedPosition;
}

inline void ModuleCamera::SetSelectedPosition(int newSelected)
{
	selectedPosition = newSelected;
}

inline float4x4 ModuleCamera::GetOrthoProjectionMatrix() const
{
	return orthoProjectionMatrix;
}
