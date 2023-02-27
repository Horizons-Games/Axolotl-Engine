#include "Camera.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "GameObject/GameObject.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentBoundingBoxes.h"
#include "Components/ComponentMeshRenderer.h"

#include "Resources/ResourceMesh.h"

#include "Windows/EditorWindows/WindowScene.h"

#include "DataStructures/Quadtree.h"

#include "Math/float3x3.h"
#include "Math/Quat.h"
#include "Geometry/Triangle.h"

Camera::Camera(const CameraType type)
	: type(type), mouseWarped(false), focusFlag(false), isFocusing(false)
{
	frustum = std::make_unique <Frustum>();
}

Camera::Camera(const std::unique_ptr<Camera>& camera, const CameraType type)
	: type(type),
	position(camera->position),
	projectionMatrix(camera->projectionMatrix),
	viewMatrix(camera->viewMatrix),
	currentRotation(camera->currentRotation),
	aspectRatio(camera->aspectRatio),
	acceleration(camera->acceleration),
	moveSpeed(camera->moveSpeed),
	rotationSpeed(camera->rotationSpeed),
	mouseSpeedModifier(camera->mouseSpeedModifier),
	frustumOffset(camera->frustumOffset),
	viewPlaneDistance(camera->viewPlaneDistance),
	frustumMode(camera->frustumMode),
	mouseWarped(camera->mouseWarped),
	focusFlag(camera->focusFlag),
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
	frustum->SetPos(position);
}

void Camera::SetOrientation(const float3& orientation)
{
	frustum->SetUp(orientation);
}

void Camera::SetLookAt(const float3& lookAt)
{
	float3 direction = lookAt - position;
	Quat finalRotation = Quat::LookAt(frustum->Front(), direction.Normalized(), frustum->Up(), float3::unitY);
	Quat nextRotation = currentRotation.Slerp(finalRotation, App->GetDeltaTime() * rotationSpeed);
	//currentRotation = rotation
	if (nextRotation.Equals(Quat::identity)) isFocusing = false;

	float3x3 rotationMatrix = float3x3::FromQuat(nextRotation);
	ApplyRotation(rotationMatrix);

}

LineSegment Camera::CreateRaycastFromMousePosition(const WindowScene* windowScene)
{
	// normalize the input to [-1, 1].
	ImVec2 startPosScene = windowScene->GetStartPos();
	ImVec2 endPosScene = windowScene->GetEndPos();

	float2 mousePositionInScene = App->input->GetMousePosition();
	mousePositionInScene.x -= startPosScene.x;
	mousePositionInScene.y -= startPosScene.y;

	float width = windowScene->GetAvailableRegion().x;
	float height = windowScene->GetAvailableRegion().y;

	float normalizedX = -1.0f + 2.0f * mousePositionInScene.x / width;
	float normalizedY = 1.0f - 2.0f * mousePositionInScene.y / height;

	return frustum->UnProjectLineSegment(normalizedX, normalizedY);
}

void Camera::CalculateHittedGameObjects(const LineSegment& ray)
{
	std::vector<GameObject*> existingGameObjects =
		App->scene->GetLoadedScene()->GetSceneGameObjects();
	std::map<float, GameObject*> hittedGameObjects;

	for (GameObject* currentGameObject : existingGameObjects)
	{
		if (currentGameObject)
		{
			float nearDistance, farDistance;
			ComponentBoundingBoxes* componentBoundingBox =
				static_cast<ComponentBoundingBoxes*>
				(currentGameObject->GetComponent(ComponentType::BOUNDINGBOX));

			bool hit = ray.Intersects(componentBoundingBox->GetEncapsuledAABB(), nearDistance, farDistance); // ray vs. AABB

			if (hit && currentGameObject->IsActive())
			{
				hittedGameObjects[nearDistance] = currentGameObject;
			}
		}
	}

	//ENGINE_LOG(std::to_string(hittedGameObjects.size()).c_str());
	SetNewSelectedGameObject(hittedGameObjects, ray);
}

void Camera::SetNewSelectedGameObject(const std::map<float, GameObject*>& hittedGameObjects,
	const LineSegment& ray)
{
	GameObject* newSelectedGameObject = nullptr;

	float thisDistance = 0.0f;
	float minCurrentDistance = inf;
	float3 exactHitPoint = float3::zero;

	for (const std::pair<float, GameObject*>& hittedGameObject : hittedGameObjects)
	{
		GameObject* actualGameObject = hittedGameObject.second;
		if (actualGameObject)
		{
			ComponentMeshRenderer* componentMeshRenderer =
				static_cast<ComponentMeshRenderer*>
				(actualGameObject->GetComponent(ComponentType::MESHRENDERER));
			std::shared_ptr<ResourceMesh> gameObjectMeshAsShared = componentMeshRenderer->GetMesh();

			if (!gameObjectMeshAsShared)
			{
				continue;
			}

			const float4x4& gameObjectModelMatrix =
				static_cast<ComponentTransform*>
				(actualGameObject->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();

			const std::vector<Triangle>& meshTriangles = gameObjectMeshAsShared->RetrieveTriangles(gameObjectModelMatrix);
			for (const Triangle& triangle : meshTriangles)
			{
				bool hit = ray.Intersects(triangle, &thisDistance, &exactHitPoint);

				if (!hit) continue;
				if (thisDistance >= minCurrentDistance) continue;

				// Only save a gameObject when any of its triangles is hit and it is the nearest triangle to the frustum
				newSelectedGameObject = actualGameObject;
				minCurrentDistance = thisDistance;
			}
		}
	}

	if (newSelectedGameObject != nullptr)
	{
		App->scene->GetLoadedScene()->GetSceneQuadTree()
			->AddGameObjectAndChildren(App->scene->GetSelectedGameObject());
		App->scene->SetSelectedGameObject(newSelectedGameObject);
		App->scene->GetLoadedScene()->GetSceneQuadTree()->RemoveGameObjectAndChildren(newSelectedGameObject);
	}
}