#pragma warning (disable: 26495)

#include "Application.h"

#include "ModuleEngineCamera.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "GameObject/GameObject.h"

#include "Components/ComponentBoundingBoxes.h"
#include "Components/ComponentBoundingBoxes.h"

#include "Resources/ResourceMesh.h"

#include "Windows/EditorWindows/WindowScene.h"

#include "Math/float3x3.h"
#include "Math/Quat.h"
#include "Geometry/Sphere.h"

ModuleEngineCamera::ModuleEngineCamera() {};

ModuleEngineCamera::~ModuleEngineCamera() {};

bool ModuleEngineCamera::Init()
{
	int w, h;

	SDL_GetWindowSize(App->window->GetWindow(), &w, &h);
	aspectRatio = float(w) / h;

	viewPlaneDistance = DEFAULT_FRUSTUM_DISTANCE;
	frustum.SetKind(FrustumProjectiveSpace::FrustumSpaceGL, FrustumHandedness::FrustumRightHanded);
	frustum.SetViewPlaneDistances(0.1f, viewPlaneDistance);
	frustum.SetHorizontalFovAndAspectRatio(math::DegToRad(90), aspectRatio);

	acceleration = DEFAULT_SHIFT_ACCELERATION;
	moveSpeed = DEFAULT_MOVE_SPEED;
	rotationSpeed = DEFAULT_ROTATION_SPEED;
	mouseSpeedModifier = DEFAULT_MOUSE_SPEED_MODIFIER;
	frustumMode = DEFAULT_FRUSTUM_MODE;
	frustumOffset = DEFAULT_FRUSTUM_OFFSET;

	position = float3(0.f, 2.f, 5.f);

	frustum.SetPos(position);
	frustum.SetFront(-float3::unitZ);
	frustum.SetUp(float3::unitY);

	if (frustumMode == offsetFrustum) RecalculateOffsetPlanes();

	return true;
}

bool ModuleEngineCamera::Start()
{
	// When the bounding boxes scale correctly with the models, uncomment this if
	/*
	if (!App->scene->GetRoot()->GetChildren().empty())
		Focus(((ComponentBoundingBoxes*)App->scene->GetRoot()->GetChildren()[0]
			->GetComponent(ComponentType::BOUNDINGBOX))->GetObjectOBB());
	*/

	return true;
}

update_status ModuleEngineCamera::Update()
{
	projectionMatrix = frustum.ProjectionMatrix();
	viewMatrix = frustum.ViewMatrix();

	if (App->editor->IsSceneFocused())
	{
		std::shared_ptr<WindowScene> windowScene = std::static_pointer_cast<WindowScene>(App->editor->GetScene());

		// --RAYCAST CALCULATION-- //
		CreateRaycastFromMousePosition(windowScene);
		CalculateHittedGameObjects();
		// --RAYCAST CALCULATION-- //

		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) != KeyState::IDLE)
			Run();
		else
			Walk();

		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) != KeyState::IDLE)
		{
			float mouseX = App->input->GetMousePosition().x - windowScene->GetStartPos().x;
			float mouseY = App->input->GetMousePosition().y - windowScene->GetStartPos().y;
		}

		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) != KeyState::IDLE)
		{
			Move();
			FreeLook();
		}

		if (App->input->IsMouseWheelScrolled())
		{
			Zoom();
		}

		if (App->scene->GetSelectedGameObject().lock() != App->scene->GetLoadedScene()->GetRoot() &&
			App->input->GetKey(SDL_SCANCODE_F) != KeyState::IDLE)
			Focus(App->scene->GetSelectedGameObject().lock());

		if (App->scene->GetSelectedGameObject().lock() != App->scene->GetLoadedScene()->GetRoot() &&
			App->input->GetKey(SDL_SCANCODE_LALT) != KeyState::IDLE &&
			App->input->GetMouseButton(SDL_BUTTON_LEFT) != KeyState::IDLE)
		{
			const OBB& obb = std::static_pointer_cast<ComponentBoundingBoxes>(
				App->scene->GetSelectedGameObject().lock()->GetComponent(ComponentType::BOUNDINGBOX))->GetObjectOBB();

			SetLookAt(obb.CenterPoint());
			Orbit(obb);
		}

		KeyboardRotate();
		if (frustumMode == offsetFrustum) RecalculateOffsetPlanes();
	}

	return UPDATE_CONTINUE;
}

void ModuleEngineCamera::Move()
{
	float deltaTime = App->GetDeltaTime();

	if (App->input->GetKey(SDL_SCANCODE_W) != KeyState::IDLE)
	{
		position = position + frustum.Front().Normalized() * 
			moveSpeed * acceleration * deltaTime;
		frustum.SetPos(position);
	}

	if (App->input->GetKey(SDL_SCANCODE_S) != KeyState::IDLE)
	{
		position = position + -(frustum.Front().Normalized()) * 
			moveSpeed * acceleration * deltaTime;
		frustum.SetPos(position);
	}

	if (App->input->GetKey(SDL_SCANCODE_A) != KeyState::IDLE)
	{
		position = position + -(frustum.WorldRight()) * moveSpeed * acceleration * deltaTime;
		frustum.SetPos(position);
	}

	if (App->input->GetKey(SDL_SCANCODE_D) != KeyState::IDLE)
	{
		position = position + frustum.WorldRight() * moveSpeed * acceleration * deltaTime;
		frustum.SetPos(position);
	}

	if (App->input->GetKey(SDL_SCANCODE_E) != KeyState::IDLE)
	{
		position = position + frustum.Up() * moveSpeed * acceleration * deltaTime;
		frustum.SetPos(position);
	}

	if (App->input->GetKey(SDL_SCANCODE_Q) != KeyState::IDLE)
	{
		position = position + -(frustum.Up()) * moveSpeed * acceleration * deltaTime;
		frustum.SetPos(position);
	}
}

void ModuleEngineCamera::KeyboardRotate()
{
	float yaw = 0.f, pitch = 0.f;

	float rotationAngle = RadToDeg(frustum.Front().Normalized().AngleBetween(float3::unitY));

	if (App->input->GetKey(SDL_SCANCODE_UP) != KeyState::IDLE)
	{
		if (rotationAngle + rotationSpeed * acceleration < 180) 
			pitch = math::DegToRad(-DEFAULT_ROTATION_DEGREE);
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) != KeyState::IDLE)
	{
		if (rotationAngle - rotationSpeed * acceleration > 0) 
			pitch = math::DegToRad(DEFAULT_ROTATION_DEGREE);
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) != KeyState::IDLE)
		yaw = math::DegToRad(DEFAULT_ROTATION_DEGREE);

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) != KeyState::IDLE)
		yaw = math::DegToRad(-DEFAULT_ROTATION_DEGREE);

	float deltaTime = App->GetDeltaTime();
	Quat pitchQuat(frustum.WorldRight(), pitch * deltaTime * rotationSpeed * acceleration);
	Quat yawQuat(float3::unitY, yaw * deltaTime * rotationSpeed * acceleration);

	float3x3 rotationMatrixX = float3x3::FromQuat(pitchQuat);
	float3x3 rotationMatrixY = float3x3::FromQuat(yawQuat);
	float3x3 rotationDeltaMatrix = rotationMatrixY * rotationMatrixX;

	ApplyRotation(rotationDeltaMatrix);
}

void ModuleEngineCamera::ApplyRotation(const float3x3& rotationMatrix) 
{
	vec oldFront = frustum.Front().Normalized();
	vec oldUp = frustum.Up().Normalized();

	frustum.SetFront(rotationMatrix.MulDir(oldFront));
	frustum.SetUp(rotationMatrix.MulDir(oldUp));
}

void ModuleEngineCamera::FreeLook()
{
	float yaw = 0.f, pitch = 0.f;
	float xrel = App->input->GetMouseMotion().x;
	float yrel = App->input->GetMouseMotion().y;
	float rotationAngle = RadToDeg(frustum.Front().Normalized().AngleBetween(float3::unitY));

	if (abs(xrel) + abs(yrel) != 0  && mouseSpeedModifier < MAX_MOUSE_SPEED_MODIFIER)
	{
		mouseSpeedModifier += 0.5f;
	}
	else if (abs(xrel) + abs(yrel) == 0)
		mouseSpeedModifier = DEFAULT_MOUSE_SPEED_MODIFIER;

	if (yrel > 0) { if (rotationAngle - yrel > 0) pitch = math::DegToRad(yrel); }
	else if (yrel < 0) { if (rotationAngle - yrel < 180) pitch = math::DegToRad(yrel); }
	
	yaw = math::DegToRad(-xrel);

	float deltaTime = App->GetDeltaTime();
	Quat pitchQuat(frustum.WorldRight(), pitch * mouseSpeedModifier * deltaTime);
	Quat yawQuat(float3::unitY, yaw * mouseSpeedModifier * deltaTime);

	float3x3 rotationMatrixX = float3x3::FromQuat(pitchQuat);
	float3x3 rotationMatrixY = float3x3::FromQuat(yawQuat);
	float3x3 rotationDeltaMatrix = rotationMatrixY * rotationMatrixX;

	ApplyRotation(rotationDeltaMatrix);
}

void ModuleEngineCamera::Run()
{
	acceleration = DEFAULT_SHIFT_ACCELERATION;
}

void ModuleEngineCamera::Walk()
{
	acceleration = 1.f;
}

void ModuleEngineCamera::Zoom()
{
	float newHFOV = GetHFOV() - App->input->GetMouseWheel().y;

	if (newHFOV <= MAX_HFOV && newHFOV >= MIN_HFOV)
		SetHFOV(math::DegToRad(newHFOV));
}

void ModuleEngineCamera::Focus(const OBB &obb)
{
	math::Sphere minSphere = obb.MinimalEnclosingSphere();

	float3 point = minSphere.Centroid();

	position = point - frustum.Front().Normalized() * minSphere.Diameter();

	SetLookAt(point);

	frustum.SetPos(position);
}

void ModuleEngineCamera::Focus(const std::shared_ptr<GameObject>& gameObject)
{
	std::list<std::weak_ptr<GameObject> > insideGameObjects = gameObject->GetGameObjectsInside();
	AABB minimalAABB;
	std::vector<math::vec> outputArray{};
	for (std::weak_ptr<GameObject> object: insideGameObjects)
	{
		std::shared_ptr<GameObject> asShared = object.lock();

		if (asShared)
		{
			std::shared_ptr<ComponentBoundingBoxes> boundingBox =
				std::static_pointer_cast<ComponentBoundingBoxes>(asShared->GetComponent(ComponentType::BOUNDINGBOX));
			outputArray.push_back(boundingBox->GetEncapsuledAABB().minPoint);
			outputArray.push_back(boundingBox->GetEncapsuledAABB().maxPoint);
		}
	}
	minimalAABB = minimalAABB.MinimalEnclosingAABB(outputArray.data(), (int)outputArray.size());
	math::Sphere minSphere = minimalAABB.MinimalEnclosingSphere();;

	if (minSphere.r == 0) minSphere.r = 1.f;
	float3 point = minSphere.Centroid();
	position = point - frustum.Front().Normalized() * minSphere.Diameter();

	SetLookAt(point);

	frustum.SetPos(position);
}


void ModuleEngineCamera::Orbit(const OBB& obb)
{
	float distance = obb.CenterPoint().Distance(position);

	vec oldPosition = position + frustum.Front().Normalized() * distance;

	float deltaTime = App->GetDeltaTime();
	Quat verticalOrbit(
		frustum.WorldRight(),
		DegToRad(-App->input->GetMouseMotion().y * rotationSpeed *
			ORBIT_SPEED_MULTIPLIER * deltaTime
		)
	);
	Quat sideOrbit(
		float3::unitY, 
		DegToRad(-App->input->GetMouseMotion().x * rotationSpeed *
			ORBIT_SPEED_MULTIPLIER * deltaTime
		)
	);

	float3x3 rotationMatrixX = float3x3::FromQuat(verticalOrbit);
	float3x3 rotationMatrixY = float3x3::FromQuat(sideOrbit);
	float3x3 rotationDeltaMatrix = rotationMatrixY * rotationMatrixX;

	ApplyRotation(rotationDeltaMatrix);

	vec newPosition = position + frustum.Front().Normalized() * distance;

	position = position + (oldPosition - newPosition);

	frustum.SetPos(position);
}

bool ModuleEngineCamera::IsInside(const AABB& aabb)
{
	if (frustumMode == noFrustum) return false;
	if (frustumMode == offsetFrustum) return IsInsideOffset(aabb);
	math::vec cornerPoints[8];
	math::Plane frustumPlanes[6];

	frustum.GetPlanes(frustumPlanes);
	aabb.GetCornerPoints(cornerPoints);

	for (int itPlanes = 0; itPlanes < 6; ++itPlanes)
	{
		bool onPlane = false;
		for (int itPoints = 0; itPoints < 8; ++itPoints)
		{
			if (!frustumPlanes[itPlanes].IsOnPositiveSide(cornerPoints[itPoints]))
			{
				onPlane = true;
				break;
			}
		}
		if (!onPlane) return false;
	}

	return true;
}

bool ModuleEngineCamera::IsInside(const OBB& obb)
{
	if (frustumMode == noFrustum) return false;
	if (frustumMode == offsetFrustum) return IsInsideOffset(obb);
	math::vec cornerPoints[8];
	math::Plane frustumPlanes[6];
	
	frustum.GetPlanes(frustumPlanes);
	obb.GetCornerPoints(cornerPoints);

	for (int itPlanes = 0; itPlanes < 6; ++itPlanes)
	{
		bool onPlane = false;
		for (int itPoints = 0 ; itPoints < 8; ++itPoints)
		{
			if (!frustumPlanes[itPlanes].IsOnPositiveSide(cornerPoints[itPoints]))
			{
				onPlane = true;
				break;
			}
		}
		if (!onPlane) return false;
	}
	
	return true;
}

bool ModuleEngineCamera::IsInsideOffset(const OBB& obb)
{
	math::vec cornerPoints[8];
	obb.GetCornerPoints(cornerPoints);

	for (int itPlanes = 0; itPlanes < 6; ++itPlanes)
	{
		bool onPlane = false;
		for (int itPoints = 0; itPoints < 8; ++itPoints)
		{
			if (!offsetFrustumPlanes[itPlanes].IsOnPositiveSide(cornerPoints[itPoints]))
			{
				onPlane = true;
				break;
			}
		}
		if (!onPlane) return false;
	}

	return true;
}

void ModuleEngineCamera::RecalculateOffsetPlanes() 
{
	math::Plane frustumPlanes[6];
	frustum.GetPlanes(frustumPlanes);

	for (int itPlanes = 0; itPlanes < 6; ++itPlanes)
	{
		math::Plane plane = frustumPlanes[itPlanes];
		plane.Translate(-frustumPlanes[itPlanes].normal * frustumOffset);
		offsetFrustumPlanes[itPlanes] = plane;
	}

}

void ModuleEngineCamera::SetHFOV(float fov)
{
	frustum.SetHorizontalFovAndAspectRatio(fov, aspectRatio);
}

void ModuleEngineCamera::SetVFOV(float fov)
{
	frustum.SetVerticalFovAndAspectRatio(fov, aspectRatio);
}

void ModuleEngineCamera::SetAspectRatio(float aspect)
{
	aspectRatio = aspect;
	SetHFOV(frustum.HorizontalFov());
}

void ModuleEngineCamera::SetPlaneDistance(float zNear, float zFar)
{
	frustum.SetViewPlaneDistances(zNear, zFar);
}

void ModuleEngineCamera::SetPosition(const float3& position)
{
	frustum.SetPos(position);
}

void ModuleEngineCamera::SetOrientation(const float3& orientation)
{
	frustum.SetUp(orientation);
}

void ModuleEngineCamera::SetLookAt(const float3& lookAt)
{
	float3 direction = lookAt - position;

	float3x3 rotationMatrix = float3x3::LookAt(
		frustum.Front().Normalized(), direction.Normalized(), frustum.Up(), float3::unitY
	);
	
	ApplyRotation(rotationMatrix);
}

void ModuleEngineCamera::SetMoveSpeed(float speed)
{
	moveSpeed = speed;
}

void ModuleEngineCamera::SetRotationSpeed(float speed)
{
	rotationSpeed = speed;
}

void ModuleEngineCamera::SetFrustumOffset(float offset)
{
	frustumOffset = offset;
}


void ModuleEngineCamera::SetFrustumMode(int mode)
{
	frustumMode = mode;
}

const float4x4& ModuleEngineCamera::GetProjectionMatrix() const
{
	return projectionMatrix;
}

const float4x4& ModuleEngineCamera::GetViewMatrix() const
{
	return viewMatrix;
}

float ModuleEngineCamera::GetHFOV() const
{
	return math::RadToDeg(frustum.HorizontalFov());
}

float ModuleEngineCamera::GetVFOV() const
{
	return math::RadToDeg(frustum.VerticalFov());
}

float ModuleEngineCamera::GetZNear() const
{
	return frustum.NearPlaneDistance();
}

float ModuleEngineCamera::GetZFar() const
{
	return frustum.FarPlaneDistance();
}

float ModuleEngineCamera::GetMoveSpeed() const
{
	return moveSpeed;
}

float ModuleEngineCamera::GetRotationSpeed() const
{
	return rotationSpeed;
}

float ModuleEngineCamera::GetDistance(const float3& point) const
{
	return frustum.Pos().Distance(point);
}

float ModuleEngineCamera::GetFrustumOffset() const 
{
	return frustumOffset;
}

int ModuleEngineCamera::GetFrustumMode() const
{
	return frustumMode;
}

void ModuleEngineCamera::CreateRaycastFromMousePosition(std::shared_ptr<WindowScene> windowScene)
{
	// normalize the input to [-1, 1].
	auto startPosScene = windowScene->GetStartPos();
	auto endPosScene = windowScene->GetEndPos();

	float2 mousePositionInScene = App->input->GetMousePosition();
	mousePositionInScene.x -= startPosScene.x;
	mousePositionInScene.y -= startPosScene.y;

	auto width = windowScene->GetAvailableRegion().x;
	auto height = windowScene->GetAvailableRegion().y;

	if (mousePositionInScene.x < 0.0f)
	{
		mousePositionInScene.x = 0.0f;
	}
	else if (mousePositionInScene.x > endPosScene.x)
	{
		mousePositionInScene.x = endPosScene.x;
	}

	if (mousePositionInScene.y < 0.0f)
	{
		mousePositionInScene.y = 0.0f;
	}
	else if (mousePositionInScene.y > endPosScene.y)
	{
		mousePositionInScene.y = endPosScene.y;
	}

	float normalizedX = -1.0 + 2.0 * mousePositionInScene.x / width;
	float normalizedY = 1.0 - 2.0 * mousePositionInScene.y / height;

	/*
	std::string log = "(";
	log += std::to_string(normalizedX).c_str();
	log += ", ";
	log += std::to_string(normalizedY).c_str();
	log += ")";
	ENGINE_LOG(log.c_str());
	*/

	ray = frustum.UnProjectLineSegment(normalizedX, normalizedY);
}

void ModuleEngineCamera::CalculateHittedGameObjects()
{
	std::vector<std::weak_ptr<GameObject>> existingGameObjects =
		App->scene->GetLoadedScene()->GetSceneGameObjects();
	std::map<float, std::weak_ptr<GameObject>> hittedGameObjects;

	for (std::weak_ptr<GameObject> currentGameObject : existingGameObjects)
	{
		float nearDistance, farDistance;
		std::shared_ptr<GameObject> currentGameObjectAsShared = currentGameObject.lock();
		std::shared_ptr<ComponentBoundingBoxes> componentBoundingBox =
			std::static_pointer_cast<ComponentBoundingBoxes>
			(currentGameObjectAsShared->GetComponent(ComponentType::BOUNDINGBOX));

		bool hit = ray.Intersects(componentBoundingBox->GetEncapsuledAABB(), nearDistance, farDistance); // ray vs. AABB

		if (hit && currentGameObjectAsShared->IsActive())
		{
			hittedGameObjects[nearDistance] = (std::weak_ptr<GameObject>(currentGameObjectAsShared));
		}
	}

	//ENGINE_LOG(std::to_string(hittedGameObjects.size()).c_str());
	SetNewSelectedGameObject(hittedGameObjects);
}

void ModuleEngineCamera::SetNewSelectedGameObject(const std::map<float, std::weak_ptr<GameObject>>& hittedGameObjects)
{
	for (std::pair<float, std::weak_ptr<GameObject>> hittedGameObject : hittedGameObjects)
	{
		std::shared_ptr<GameObject> hittedAsShared = hittedGameObject.second.lock();

		//ENGINE_LOG(hittedAsShared->GetName());
	}
}