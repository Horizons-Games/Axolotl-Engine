#include "CameraGameObject.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModulePlayer.h"
#include "ModuleWindow.h"

CameraGameObject::CameraGameObject() : Camera(CameraType::C_GAMEOBJECT)
{
}

CameraGameObject::CameraGameObject(const std::unique_ptr<Camera>& camera) : Camera(camera, CameraType::C_GAMEOBJECT)
{
}

CameraGameObject::~CameraGameObject()
{
}

bool CameraGameObject::Update()
{
	projectionMatrix = frustum->ProjectionMatrix();
	viewMatrix = frustum->ViewMatrix();

	if (App->GetModule<ModuleInput>()->GetInFocus())
	{
		if (!App->GetModule<ModulePlayer>()->IsStatic() && !SDL_ShowCursor(SDL_QUERY))
		{
			UpdateCameraWithMousePos();
		}
	}
	return true;
}

void CameraGameObject::SetPositionTarget(const float3& targetPosition, float deltaTime)
{
	float3 positionError = targetPosition - position;

	float3 velocityPosition = positionError * KpPosition;
	float3 translationVector = velocityPosition * deltaTime;
	if (translationVector.Length() > positionError.Length())
	{
		SetPosition(targetPosition);
	}
	else
	{
		float3 nextPos = position + translationVector;
		SetPosition(nextPos);
	}
}

void CameraGameObject::SetRotationTarget(const Quat& targetRotation, float deltaTime)
{

	Quat rotationError = targetRotation * rotation.Inverted();
	rotationError.Normalize();

	float3 axis;
	float angle;
	rotationError.ToAxisAngle(axis, angle);
	axis.Normalize();

	float3 velocityRotation = axis * angle * KpRotation;
	Quat angularVelocityQuat(velocityRotation.x, velocityRotation.y, velocityRotation.z, 0.0f);
	Quat wq_0 = angularVelocityQuat * rotation;

	float deltaValue = 0.5f * deltaTime;
	Quat deltaRotation = Quat(deltaValue * wq_0.x, deltaValue * wq_0.y, deltaValue * wq_0.z, deltaValue * wq_0.w);

	if (deltaRotation.Length() > rotationError.Length())
	{
		ApplyRotationWithFixedUp(targetRotation, float3::unitY);
		// ApplyRotation(targetRotation);
	}

	else
	{
		Quat nextRotation(rotation.x + deltaRotation.x,
						  rotation.y + deltaRotation.y,
						  rotation.z + deltaRotation.z,
						  rotation.w + deltaRotation.w);
		nextRotation.Normalize();

		ApplyRotationWithFixedUp(nextRotation, float3::unitY);
		// ApplyRotation(nextRotation);
	}
}

void CameraGameObject::ApplyRotation(const Quat& rotationQuat)
{
	frustum->SetFront(rotationQuat.Transform(float3::unitZ));
	frustum->SetUp(rotationQuat.Transform(float3::unitY));

	rotation = rotationQuat;
}

void CameraGameObject::ApplyRotationWithFixedUp(const Quat& rotationQuat, const float3& fixedUp)
{
	float3 newFront = rotationQuat.Transform(float3::unitZ);
	float3 newRight = fixedUp.Cross(newFront).Normalized();
	float3 newUp = newFront.Cross(newRight);

	frustum->SetFront(newFront);
	frustum->SetUp(newUp);

	rotation = rotationQuat;
}

void CameraGameObject::UpdateCameraWithMousePos()
{
	ModuleInput* input = App->GetModule<ModuleInput>();

	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	input->SetMouseMotionX(float(mouseX - lastMouseX));
	input->SetMouseMotionY(float(mouseY - lastMouseY));

	int width, height;
	SDL_Window* sdlWindow = App->GetModule<ModuleWindow>()->GetWindow();
	SDL_GetWindowSize(sdlWindow, &width, &height);
	SDL_WarpMouseInWindow(sdlWindow, width / 2, height / 2);

	lastMouseX = width / 2;
	lastMouseY = height / 2;
}