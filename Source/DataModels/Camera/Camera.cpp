#include "Camera.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "GameObject/GameObject.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentBoundingBoxes.h"
#include "Components/ComponentMeshRenderer.h"

#include "Resources/ResourceMesh.h"

#include "Windows/EditorWindows/WindowScene.h"

#include "Math/float3x3.h"
#include "Math/Quat.h"
#include "Geometry/Sphere.h"
#include "Geometry/Triangle.h"

Camera::~Camera()
{
}

void Camera::ApplyRotation(const float3x3& rotationMatrix)
{
	vec oldFront = frustum->Front().Normalized();
	vec oldUp = frustum->Up().Normalized();

	frustum->SetFront(rotationMatrix.MulDir(oldFront));
	frustum->SetUp(rotationMatrix.MulDir(oldUp));
}

bool Camera::IsInside(const AABB& aabb)
{
	if (frustumMode == noFrustum) return false;
	if (frustumMode == offsetFrustum) return IsInsideOffset(aabb);
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
	if (frustumMode == noFrustum) return false;
	if (frustumMode == offsetFrustum) return IsInsideOffset(obb);
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

void Camera::SetMoveSpeed(float speed)
{
	moveSpeed = speed;
}

void Camera::SetRotationSpeed(float speed)
{
	rotationSpeed = speed;
}

void Camera::SetFrustumOffset(float offset)
{
	frustumOffset = offset;
}


void Camera::SetFrustumMode(int mode)
{
	frustumMode = mode;
}

const float4x4& Camera::GetProjectionMatrix() const
{
	return projectionMatrix;
}

const float4x4& Camera::GetViewMatrix() const
{
	return viewMatrix;
}

float Camera::GetHFOV() const
{
	return math::RadToDeg(frustum->HorizontalFov());
}

float Camera::GetVFOV() const
{
	return math::RadToDeg(frustum->VerticalFov());
}

float Camera::GetZNear() const
{
	return frustum->NearPlaneDistance();
}

float Camera::GetZFar() const
{
	return frustum->FarPlaneDistance();
}

float Camera::GetMoveSpeed() const
{
	return moveSpeed;
}

float Camera::GetRotationSpeed() const
{
	return rotationSpeed;
}

float Camera::GetDistance(const float3& point) const
{
	return frustum->Pos().Distance(point);
}

float Camera::GetFrustumOffset() const
{
	return frustumOffset;
}

int Camera::GetFrustumMode() const
{
	return frustumMode;
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