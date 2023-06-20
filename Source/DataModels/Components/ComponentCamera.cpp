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

ComponentCamera::ComponentCamera(bool active, GameObject* owner) :
	Component(ComponentType::CAMERA, active, owner, false)
{
	camera = std::make_unique<CameraGameObject>();
	camera->Init();
	camera->SetKpPosition(5.0f);
	camera->SetKpRotation(2.0f);
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
	ComponentTransform* trans = GetOwner()->GetComponent<ComponentTransform>();
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

void ComponentCamera::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool) active;
	meta["removed"] = (bool) canBeRemoved;

	meta["frustumOfset"] = camera->GetFrustumOffset();
	meta["drawFrustum"] = camera->IsDrawFrustum();
	// meta["frustumMode"] = camera->GetFrustumMode();
}

void ComponentCamera::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool) meta["active"];
	canBeRemoved = (bool) meta["removed"];

	camera->SetFrustumOffset((float) meta["frustumOfset"]);
	camera->SetIsDrawFrustum((bool) meta["drawFrustum"]);
	// frustumMode = GetFrustumModeByName(meta["frustumMode"]);
}

void ComponentCamera::DuplicateCamera(CameraGameObject* camera)
{
	this->camera = std::make_unique<CameraGameObject>(static_cast<CameraGameObject&>(*camera));
}
