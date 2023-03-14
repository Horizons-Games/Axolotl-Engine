#pragma once
#pragma warning (disable: 26495)
#pragma warning (disable: 4005)

#include "Module.h"
#include "ModuleDebugDraw.h"
#include "Camera/Camera.h"

#define DEFAULT_MOVE_SPEED 9.f
#define DEFAULT_ROTATION_DEGREE 30
#define DEFAULT_ROTATION_SPEED 5.f
#define DEFAULT_MOUSE_SPEED_MODIFIER 0.f
#define DEFAULT_SHIFT_ACCELERATION 2.f
#define DEFAULT_FRUSTUM_MODE 0
#define DEFAULT_FRUSTUM_OFFSET 1.f
#define DEFAULT_FRUSTUM_DISTANCE 20000.f

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

class ModuleCamera : public Module
{
public:
	ModuleCamera();
	~ModuleCamera() override;

	bool Init() override;
	bool Start() override;

	update_status Update();

	Camera* GetCamera();
	void ChangeCamera(CameraType newType);
	Camera* GetSelectedCamera() const;
	void SetSelectedCamera(int cameraNumber);

	
private:
	std::unique_ptr <Camera> camera;
	Camera* selectedCamera;
	int selectedPosition;
};


inline Camera* ModuleCamera::GetSelectedCamera() const
{
	return selectedCamera;
}