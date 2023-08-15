#include "StdAfx.h"

#include "ComponentCamera.h"

#include "Application.h"
#include "ModuleDebugDraw.h"

#include "Math/float3x3.h"

#include "ComponentTransform.h"
#include "FileSystem/Json.h"
#include "GameObject/GameObject.h"
#include "Math/Quat.h"

#include "Camera/CameraEngine.h"
#include "Camera/CameraGameObject.h"
#include "Camera/CameraGod.h"

#include "Geometry/Frustum.h"

ComponentCamera::ComponentCamera(bool active, GameObject* owner) :
	Component(ComponentType::CAMERA, active, owner, false)
{
	camera = std::make_unique<CameraGameObject>();
	camera->Init();
	camera->SetKpPosition(5.0f);
	camera->SetKpRotation(5.0f);
	KpPosition = 5.0f;
	KpRotation = 5.0f;
	camera->SetViewPlaneDistance(DEFAULT_GAMEOBJECT_FRUSTUM_DISTANCE);
	Update();
}

ComponentCamera::ComponentCamera(const ComponentCamera& componentCamera) : Component(componentCamera)
{
	DuplicateCamera(componentCamera.camera.get());
}

ComponentCamera::~ComponentCamera()
{
}

void ComponentCamera::Update()
{
	float deltaTime = App->GetDeltaTime();
	ComponentTransform* trans = GetOwner()->GetComponentInternal<ComponentTransform>();
	camera->SetPositionTarget(trans->GetGlobalPosition(), deltaTime);

	Quat rotation = trans->GetGlobalRotation();
	camera->SetRotationTarget(rotation, deltaTime);

	if (camera->GetFrustumMode() == EFrustumMode::offsetFrustum)
	{
		camera->RecalculateOffsetPlanes();
	}
}

void ComponentCamera::Draw() const
{
#ifdef ENGINE
	if (camera->IsDrawFrustum())
		App->GetModule<ModuleDebugDraw>()->DrawFrustum(*camera->GetFrustum());
#endif // ENGINE
}

void ComponentCamera::SetSampleKpPosition(float kp)
{
	camera->SetKpPosition(kp);
}

void ComponentCamera::SetSampleKpRotation(float kp)
{
	camera->SetKpRotation(kp);
}

void ComponentCamera::RestoreKpPosition()
{
	camera->SetKpPosition(KpPosition);
}

void ComponentCamera::RestoreKpRotation()
{
	camera->SetKpRotation(KpRotation);
}

void ComponentCamera::InternalSave(Json& meta)
{
	meta["frustumOfset"] = camera->GetFrustumOffset();
	meta["drawFrustum"] = camera->IsDrawFrustum();
	meta["kpPosition"] = camera->GetKpPosition();
	meta["kpRotation"] = camera->GetKpRotation();
	// meta["frustumMode"] = camera->GetFrustumMode();
}

void ComponentCamera::InternalLoad(const Json& meta)
{
	camera->SetFrustumOffset((float) meta["frustumOfset"]);
	camera->SetIsDrawFrustum((bool) meta["drawFrustum"]);
	camera->SetKpPosition((float) meta["kpPosition"]);
	camera->SetKpRotation((float) meta["kpRotation"]);

	KpPosition = camera->GetKpPosition();
	KpRotation = camera->GetKpRotation();
	// frustumMode = GetFrustumModeByName(meta["frustumMode"]);
}

void ComponentCamera::DuplicateCamera(CameraGameObject* camera)
{
	this->camera = std::make_unique<CameraGameObject>(static_cast<CameraGameObject&>(*camera));
}


