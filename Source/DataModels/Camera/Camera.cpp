#include "Camera.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "GameObject/GameObject.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentMeshRenderer.h"

#include "Resources/ResourceMesh.h"

#include "Windows/EditorWindows/WindowScene.h"

#include "DataStructures/Quadtree.h"

#include "Math/float3x3.h"
#include "Math/Quat.h"
#include "Geometry/Triangle.h"

Camera::Camera(const CameraType type)
	: type(type), mouseWarped(false), isFocusing(false), interpolationTime(0.0f), interpolationDuration(5.0f)
{
	frustum = std::make_unique <Frustum>();
}

Camera::Camera(Camera& camera)
	: type(type),
	position(camera.position),
	projectionMatrix(camera.projectionMatrix),
	viewMatrix(camera.viewMatrix),
	aspectRatio(camera.aspectRatio),
	acceleration(camera.acceleration),
	moveSpeed(camera.moveSpeed),
	rotationSpeed(camera.rotationSpeed),
	mouseSpeedModifier(camera.mouseSpeedModifier),
	frustumOffset(camera.frustumOffset),
	viewPlaneDistance(camera.viewPlaneDistance),
	frustumMode(camera.frustumMode),
	mouseWarped(camera.mouseWarped),
	interpolationTime(camera.interpolationTime),
	interpolationDuration(camera.interpolationDuration),
	isFocusing(camera.isFocusing),
	lastMouseX(camera.lastMouseX),
	lastMouseY(camera.lastMouseY),
	mouseState(camera.mouseState),
	frustum(std::move(camera.frustum))
{
	if (frustumMode == EFrustumMode::offsetFrustum)
	{
		RecalculateOffsetPlanes();
	}
}

Camera::Camera(const std::unique_ptr<Camera>& camera, const CameraType type)
	: type(type),
	position(camera->position),
	projectionMatrix(camera->projectionMatrix),
	viewMatrix(camera->viewMatrix),
	aspectRatio(camera->aspectRatio),
	acceleration(camera->acceleration),
	moveSpeed(camera->moveSpeed),
	rotationSpeed(camera->rotationSpeed),
	mouseSpeedModifier(camera->mouseSpeedModifier),
	frustumOffset(camera->frustumOffset),
	viewPlaneDistance(camera->viewPlaneDistance),
	frustumMode(camera->frustumMode),
	mouseWarped(camera->mouseWarped),
	interpolationDuration(camera->interpolationDuration),
	isFocusing(camera->isFocusing),
	lastMouseX(camera->lastMouseX),
	lastMouseY(camera->lastMouseY),
	mouseState(camera->mouseState),
	frustum(std::move(camera->frustum))
{
	//frustum = std::make_unique <Frustum>();
	if (frustumMode == EFrustumMode::offsetFrustum)
	{
		RecalculateOffsetPlanes();
	}
}

Camera::~Camera()
{
}

bool Camera::Init()
{
	int w, h;
	SDL_GetWindowSize(App->window->GetWindow(), &w, &h);
	aspectRatio = float(w) / h;

	viewPlaneDistance = DEFAULT_FRUSTUM_DISTANCE;
	frustum->SetKind(FrustumProjectiveSpace::FrustumSpaceGL, FrustumHandedness::FrustumRightHanded);
	frustum->SetViewPlaneDistances(0.1f, viewPlaneDistance);
	frustum->SetHorizontalFovAndAspectRatio(math::DegToRad(90), aspectRatio);

	acceleration = DEFAULT_SHIFT_ACCELERATION;
	moveSpeed = DEFAULT_MOVE_SPEED;
	rotationSpeed = DEFAULT_ROTATION_SPEED;
	mouseSpeedModifier = DEFAULT_MOUSE_SPEED_MODIFIER;
	frustumMode = DEFAULT_FRUSTUM_MODE;
	frustumOffset = DEFAULT_FRUSTUM_OFFSET;

	position = float3(0.f, 2.f, 5.f);

	frustum->SetPos(position);
	frustum->SetFront(-float3::unitZ);
	frustum->SetUp(float3::unitY);

	if (frustumMode == EFrustumMode::offsetFrustum)
	{
		RecalculateOffsetPlanes();
	}

	return true;
}

bool Camera::Start()
{
	return true;
}

void Camera::ApplyRotation(const float3x3& rotationMatrix)
{
	vec oldFront = frustum->Front().Normalized();
	vec oldUp = frustum->Up().Normalized();

	frustum->SetFront(rotationMatrix.MulDir(oldFront));
	frustum->SetUp(rotationMatrix.MulDir(oldUp));
}

void Camera::Run()
{
	acceleration = DEFAULT_SHIFT_ACCELERATION;
}

void Camera::Walk()
{
	acceleration = 1.f;
}

void Camera::KeyboardRotate()
{
	float yaw = 0.f, pitch = 0.f;

	float rotationAngle = RadToDeg(frustum->Front().Normalized().AngleBetween(float3::unitY));

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
	Quat pitchQuat(frustum->WorldRight(), pitch * deltaTime * rotationSpeed * acceleration);
	Quat yawQuat(float3::unitY, yaw * deltaTime * rotationSpeed * acceleration);

	float3x3 rotationMatrixX = float3x3::FromQuat(pitchQuat);
	float3x3 rotationMatrixY = float3x3::FromQuat(yawQuat);
	float3x3 rotationDeltaMatrix = rotationMatrixY * rotationMatrixX;

	ApplyRotation(rotationDeltaMatrix);
}

void Camera::FreeLook()
{
	float deltaTime = App->GetDeltaTime();
	float mouseSpeedPercentage = 0.05f;
	float xrel = -App->input->GetMouseMotion().x * (rotationSpeed * mouseSpeedPercentage) * deltaTime;
	float yrel = -App->input->GetMouseMotion().y * (rotationSpeed * mouseSpeedPercentage) * deltaTime;

	float3x3 x = float3x3(Cos(xrel), 0.0f, Sin(xrel), 0.0f, 1.0f, 0.0f, -Sin(xrel), 0.0f, Cos(xrel));
	float3x3 y = float3x3::RotateAxisAngle(frustum->WorldRight().Normalized(), yrel);
	float3x3 xy = x * y;

	vec oldUp = frustum->Up().Normalized();
	vec oldFront = frustum->Front().Normalized();

	float3 newUp = xy.MulDir(oldUp);

	if (newUp.y > 0.f)
	{
		frustum->SetUp(xy.MulDir(oldUp));
		frustum->SetFront(xy.MulDir(oldFront));
	}
	else
	{
		y = float3x3::RotateAxisAngle(frustum->WorldRight().Normalized(), 0);
		xy = x * y;

		frustum->SetUp(xy.MulDir(oldUp));
		frustum->SetFront(xy.MulDir(oldFront));
	}
}

bool Camera::IsInside(const AABB& aabb)
{
	if (frustumMode == EFrustumMode::noFrustum)
	{
		return false;
	}
	if (frustumMode == EFrustumMode::offsetFrustum)
	{
		return IsInsideOffset(aabb);
	}
	math::vec cornerPoints[8];
	math::Plane frustumPlanes[6];

	frustum->GetPlanes(frustumPlanes);
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

bool Camera::IsInside(const OBB& obb)
{
	if (frustumMode == EFrustumMode::noFrustum) 
	{
		return false;
	} 
	if (frustumMode == EFrustumMode::offsetFrustum) 
	{
		return IsInsideOffset(obb);
	}
	math::vec cornerPoints[8];
	math::Plane frustumPlanes[6];

	frustum->GetPlanes(frustumPlanes);
	obb.GetCornerPoints(cornerPoints);

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

bool Camera::IsInsideOffset(const OBB& obb)
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

void Camera::RecalculateOffsetPlanes()
{
	math::Plane frustumPlanes[6];
	frustum->GetPlanes(frustumPlanes);

	for (int itPlanes = 0; itPlanes < 6; ++itPlanes)
	{
		math::Plane plane = frustumPlanes[itPlanes];
		plane.Translate(-frustumPlanes[itPlanes].normal * frustumOffset);
		offsetFrustumPlanes[itPlanes] = plane;
	}

}

void Camera::SetHFOV(float fov)
{
	frustum->SetHorizontalFovAndAspectRatio(fov, aspectRatio);
}

void Camera::SetVFOV(float fov)
{
	frustum->SetVerticalFovAndAspectRatio(fov, aspectRatio);
}

void Camera::SetAspectRatio(float aspect)
{
	aspectRatio = aspect;
	SetHFOV(frustum->HorizontalFov());
}

void Camera::SetPlaneDistance(float zNear, float zFar)
{
	frustum->SetViewPlaneDistances(zNear, zFar);
}

void Camera::SetPosition(const float3& position)
{
	this->position = position;
	frustum->SetPos(position);
}

void Camera::SetOrientation(const float3& orientation)
{
	frustum->SetUp(orientation);
}

void Camera::SetLookAt(const float3& lookAt, float interpolationTime)
{
	float3 targetDirection = (lookAt - position).Normalized();
	float3 currentDirection = frustum->Front().Normalized();

	float3 nextDirection = Quat::SlerpVector(currentDirection, targetDirection, interpolationTime / interpolationDuration);
	Quat nextRotation = Quat::LookAt(frustum->Front(), nextDirection.Normalized(), frustum->Up(), float3::unitY);
	float3x3 rotationMatrix = float3x3::FromQuat(nextRotation);
	ApplyRotation(rotationMatrix);
	ENGINE_LOG("s(t): (%.6f, %.6f, %.6f", nextDirection.x, nextDirection.y, nextDirection.z);

}

void Camera::SetNewSelectedGameObject(GameObject* gameObject)
{
	if (gameObject != nullptr)
	{
		App->scene->ChangeSelectedGameObject(gameObject);
		App->scene->GetSelectedGameObject()->SetStateOfSelection(StateOfSelection::SELECTED);
	}
}