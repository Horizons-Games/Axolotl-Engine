#pragma once
#include "Component.h"
#include "Auxiliar/Generics/Drawable.h"
#include "Auxiliar/Generics/Updatable.h"

#include "Geometry/Frustum.h"
#include "Geometry/Plane.h"
#include "Geometry/OBB.h"

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
class CameraGameObject;
class ComponentTransform;

class ComponentCamera : public Component, public Updatable, public Drawable
{
public:
	ComponentCamera(bool active, GameObject* owner);
	ComponentCamera(const ComponentCamera& componentCamera);
	~ComponentCamera() override;

	void Update() override;
	void Draw() const override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	CameraGameObject* GetCamera();

	void DuplicateCamera(CameraGameObject* camera);


private:
	std::unique_ptr <CameraGameObject> camera;
};

inline CameraGameObject* ComponentCamera::GetCamera()
{
	return camera.get();
}

