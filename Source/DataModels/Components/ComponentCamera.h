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

	void OnTransformChanged() override;

	CameraGameObject* GetCamera();

	void DuplicateCamera(CameraGameObject* camera);

	void SetSampleKpPosition(float kp);
	void SetSampleKpRotation(float kp);

	void RestoreKpPosition();
	void RestoreKpRotation();

	float GetKpPosition() const;
	void SetKpPosition(float kp);
	float GetKpRotation() const;
	void SetKpRotation(float kp);

	bool IsFrustumChecked() const;
	void SetFrustumChecked(bool checked);

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

private:
	std::unique_ptr<CameraGameObject> camera;
	float KpPosition;
	float KpRotation;

	bool isFrustumChecked;
};

inline CameraGameObject* ComponentCamera::GetCamera()
{
	return camera.get();
}

inline float ComponentCamera::GetKpPosition() const
{
	return KpPosition;
}

inline void ComponentCamera::SetKpPosition(float kp)
{
	KpPosition = kp;
}

inline float ComponentCamera::GetKpRotation() const
{
	return KpRotation;
}

inline void ComponentCamera::SetKpRotation(float kp)
{
	KpRotation = kp;
}

inline bool ComponentCamera::IsFrustumChecked() const
{
	return isFrustumChecked;
}

inline void ComponentCamera::SetFrustumChecked(bool checked)
{
	isFrustumChecked = checked;
}
