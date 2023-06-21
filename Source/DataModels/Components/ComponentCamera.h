#pragma once
#include "Auxiliar/Generics/Drawable.h"
#include "Auxiliar/Generics/Updatable.h"
#include "Component.h"

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

	CameraGameObject* GetCamera();

	void DuplicateCamera(CameraGameObject* camera);

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

private:
	std::unique_ptr<CameraGameObject> camera;
};

inline CameraGameObject* ComponentCamera::GetCamera()
{
	return camera.get();
}
