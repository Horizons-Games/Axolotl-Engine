#pragma once
#include "Component.h"

#include "Geometry/Frustum.h"
#include "Geometry/Plane.h"
#include "Geometry/OBB.h"

class CameraGameObject;

#define COMPONENT_CAMERA "Camera"

/*
enum class ECameraFrustumMode
{
	UNKNOWN,
	NORMALFRUSTUM,
	OFFSETFRUSTUM,
	NOFRUSTUM
};
const static std::string GetNameByFrustumMode(ECameraFrustumMode type);
const static ECameraFrustumMode GetFrustumModeByName(const std::string& name);
*/

class Json;
class ComponentTransform;

class ComponentCamera : public Component
{
public:
	ComponentCamera(bool active, GameObject* owner);
	~ComponentCamera() override;

	void Update() override;
	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	CameraGameObject* GetCamera();


private:
	std::unique_ptr <CameraGameObject> camera;
};
