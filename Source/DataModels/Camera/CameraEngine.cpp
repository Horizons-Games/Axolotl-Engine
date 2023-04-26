#include "CameraEngine.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "GameObject/GameObject.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentMeshRenderer.h"

#include "Resources/ResourceMesh.h"

#include "Windows/EditorWindows/WindowScene.h"

#include "Math/float3x3.h"
#include "Math/Quat.h"
#include "Geometry/Sphere.h"
#include "Geometry/Triangle.h"
#include "Physics/Physics.h"

CameraEngine::CameraEngine() 
: Camera(CameraType::C_ENGINE)
{
	currentFocusDir = frustum->Front().Normalized();
	currentFocusPos = position;
};

CameraEngine::CameraEngine(const std::unique_ptr<Camera>& camera)
	: Camera(camera, CameraType::C_ENGINE)
{
	currentFocusDir = frustum->Front().Normalized();
	currentFocusPos = position;
}

CameraEngine::~CameraEngine()
{
};

bool CameraEngine::Update()
{
	projectionMatrix = frustum->ProjectionMatrix();
	viewMatrix = frustum->ViewMatrix();

	App->GetModule<ModuleInput>()->SetDefaultCursor();

	bool sceneFocused = App->GetModule<ModuleEditor>()->IsSceneFocused();

	if (sceneFocused && !App->IsOnPlayMode())
	{
		
		if (isFocusing)
		{
			Focus(App->GetModule<ModuleScene>()->GetSelectedGameObject());
		}
		else
		{
			//Shift speed
			if (App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_LSHIFT) != KeyState::IDLE)
				Run();
			else
				Walk();

			//this should probably be encapsulated in a method, or moved to the Physics part of the Engine
			// --RAYCAST CALCULATION-- //
			if (App->GetModule<ModuleInput>()->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::DOWN &&
				App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_LALT) == KeyState::IDLE)
			{
				LineSegment ray;
				if (Physics::ScreenPointToRay(App->GetModule<ModuleInput>()->GetMousePosition(), ray))
				{
					RaycastHit hit;
					if (Physics::Raycast(ray, hit)) 
					{
						SetNewSelectedGameObject(hit.gameObject);
					}
				}
			}
			// --RAYCAST CALCULATION-- //

			//Move and rotate with right buttons and ASDWQE
			if (App->GetModule<ModuleInput>()->GetMouseButton(SDL_BUTTON_RIGHT) != KeyState::IDLE &&
				App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_LALT) == KeyState::IDLE)
			{
				App->GetModule<ModuleInput>()->SetFreeLookCursor();
				UnlimitedCursor();
				Move();
				FreeLook();
			}

			//Zoom with mouse wheel
			if (App->GetModule<ModuleInput>()->IsMouseWheelScrolled() && App->GetModule<ModuleInput>()->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::IDLE)
			{
				Zoom();
			}

			//Move camera UP/DOWN and RIGHT/LEFT with mouse mid button
			if (App->GetModule<ModuleInput>()->GetMouseButton(SDL_BUTTON_MIDDLE) != KeyState::IDLE)
			{
				App->GetModule<ModuleInput>()->SetMoveCursor();
				UnlimitedCursor();
				Move();
			}

			//Focus
			if (App->GetModule<ModuleScene>()->GetSelectedGameObject() != App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot() &&
				App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_F) != KeyState::IDLE)
			{
				if (!isUsingProportionalController) {
					currentFocusDir = frustum->Front().Normalized();
					currentFocusPos = position;
				}
				isFocusing = true;
			}

			//Orbit object with ALT + LEFT MOUSE CLICK
			if (App->GetModule<ModuleScene>()->GetSelectedGameObject() != App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot() &&
				App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_LALT) != KeyState::IDLE &&
				App->GetModule<ModuleInput>()->GetMouseButton(SDL_BUTTON_LEFT) != KeyState::IDLE)
			{
				ComponentTransform* transform =
					static_cast<ComponentTransform*>(App->GetModule<ModuleScene>()->GetSelectedGameObject()->GetComponent(ComponentType::TRANSFORM));
				const OBB& obb = transform->GetObjectOBB();

				App->GetModule<ModuleInput>()->SetOrbitCursor();
				UnlimitedCursor();
				Orbit(obb);
			}

			//Zoom with ALT + RIGHT MOUSE CLICK
			if (App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_LALT) != KeyState::IDLE &&
				App->GetModule<ModuleInput>()->GetMouseButton(SDL_BUTTON_RIGHT) != KeyState::IDLE &&
				App->GetModule<ModuleInput>()->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::IDLE) //Not pressing mouse left button
			{
				App->GetModule<ModuleInput>()->SetZoomCursor();
				UnlimitedCursor();
				Zoom();
			}

			KeyboardRotate();

			if (frustumMode == EFrustumMode::offsetFrustum)
			{
				RecalculateOffsetPlanes();
			}
		}
	}

	return true;
}

void CameraEngine::Move()
{
	//Increase/decrease camera velocity with mouse wheel
	if (App->GetModule<ModuleInput>()->IsMouseWheelScrolled() && App->GetModule<ModuleInput>()->GetMouseButton(SDL_BUTTON_MIDDLE) == KeyState::IDLE)
	{
		moveSpeed += App->GetModule<ModuleInput>()->GetMouseWheel().y;
		if (moveSpeed < 1.0f)
		{
			moveSpeed = 1.0f;
		}
		if (moveSpeed > 900.0f)
		{
			moveSpeed = 900.0f;
		}
	}

	//Forward
	if (App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_W) != KeyState::IDLE)
	{
		position += frustum->Front().Normalized() *
			moveSpeed * acceleration * App->GetDeltaTime();
		SetPosition(position);
	}

	//Backward
	if (App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_S) != KeyState::IDLE)
	{
		position += -(frustum->Front().Normalized()) *
			moveSpeed * acceleration * App->GetDeltaTime();
		SetPosition(position);
	}

	//Left
	if (App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_A) != KeyState::IDLE)
	{
		position += -(frustum->WorldRight()) * moveSpeed * acceleration * App->GetDeltaTime();
		SetPosition(position);
	}

	//Right
	if (App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_D) != KeyState::IDLE)
	{
		position += frustum->WorldRight() * moveSpeed * acceleration * App->GetDeltaTime();
		SetPosition(position);
	}

	//Up
	if (App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_E) != KeyState::IDLE)
	{
		position += frustum->Up() * moveSpeed * acceleration * App->GetDeltaTime();
		SetPosition(position);
	}

	//Down
	if (App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_Q) != KeyState::IDLE)
	{
		position += -(frustum->Up()) * moveSpeed * acceleration * App->GetDeltaTime();
		SetPosition(position);
	}

	//Move UP/DOWN and RIGHT/LEFT with mid mouse button
	if (App->GetModule<ModuleInput>()->GetMouseButton(SDL_BUTTON_MIDDLE) != KeyState::IDLE)
	{
		float mouseSpeedPercentage = 0.05f;
		float xrel = -App->GetModule<ModuleInput>()->GetMouseMotion().x * (rotationSpeed * mouseSpeedPercentage) * App->GetDeltaTime();
		float yrel = App->GetModule<ModuleInput>()->GetMouseMotion().y * (rotationSpeed * mouseSpeedPercentage) * App->GetDeltaTime();

		position += (frustum->WorldRight()) * xrel;
		position += (frustum->Up()) * yrel;
		SetPosition(position);
	}
}

void CameraEngine::Zoom()
{
	if (App->GetModule<ModuleInput>()->IsMouseWheelScrolled())
	{
		float zoomSpeed = App->GetModule<ModuleInput>()->GetMouseWheel().y * DEFAULT_MOUSE_ZOOM_SPEED;

		position += frustum->Front().Normalized() *
			zoomSpeed * App->GetDeltaTime();
	}
	else
	{
		float zoomSpeed = App->GetModule<ModuleInput>()->GetMouseMotion().x * DEFAULT_MOUSE_ZOOM_SPEED;

		position += frustum->Front().Normalized() *
			zoomSpeed * App->GetDeltaTime();
	}
	SetPosition(position);
}

void CameraEngine::FocusInterpolated(const OBB& obb)
{
	Sphere boundingSphere = obb.MinimalEnclosingSphere();

	float radius = boundingSphere.r;
	if (boundingSphere.r < 1.f) radius = 1.f;
	float fov = frustum->HorizontalFov();
	float camDistance = radius / float(sin(fov / 2.0));
	float3 camDirection = (boundingSphere.pos - currentFocusPos).Normalized();

	float3 targetDirection = (boundingSphere.pos - position).Normalized();
	float3 endposition = boundingSphere.pos - (camDirection * camDistance);

	if (currentFocusPos.Equals(endposition) && currentFocusDir.Equals(camDirection))
	{
		interpolationTime = 0.0f;
		isFocusing = false;
	}
	else 
	{
		interpolationTime += App->GetDeltaTime();

		float currentTimeRelation = interpolationTime / interpolationDuration;

		if (currentTimeRelation >= 1.0f)
		{
			Quat nextRotation = Quat::LookAt(frustum->Front(), targetDirection.Normalized(), frustum->Up(), float3::unitY);
			ApplyRotation(nextRotation);

			SetPosition(endposition);

			currentFocusPos = endposition;
			currentFocusDir = camDirection;
			
			interpolationTime = 0.0f;
			isFocusing = false;
		}
		else 
		{
			if (currentFocusPos.Equals(endposition))
			{
				float3 nextDirection = Quat::SlerpVector(currentFocusDir, targetDirection, currentTimeRelation);
				Quat nextRotation = Quat::LookAt(frustum->Front(), nextDirection.Normalized(), frustum->Up(), float3::unitY);
				ApplyRotation(nextRotation);
			}
			else if (currentFocusDir.Equals(camDirection))
			{
				float3 nextPos = currentFocusPos.Lerp(endposition, currentTimeRelation);
				SetPosition(nextPos);
			}
			else
			{
				float3 nextDirection = Quat::SlerpVector(currentFocusDir, targetDirection, currentTimeRelation);
				Quat nextRotation = Quat::LookAt(frustum->Front(), nextDirection.Normalized(), frustum->Up(), float3::unitY);
				ApplyRotation(nextRotation);

				float3 nextPos = currentFocusPos.Lerp(endposition, currentTimeRelation);
				SetPosition(nextPos);
			}
		}
		
	}
	
}
void CameraEngine::FocusProportional(const OBB& obb)
{
	Sphere boundingSphere = obb.MinimalEnclosingSphere();

	float radius = boundingSphere.r;
	if (boundingSphere.r < 1.f) radius = 1.f;
	float fov = frustum->HorizontalFov();
	float camDistance = radius / float(sin(fov / 2.0));

	bool positionAchieved = false;
	bool rotationAchieved = false;
	Quat targetRotation = Quat::identity;

	float3 camDirection = (boundingSphere.pos - position).Normalized();
	float3 localForward = frustum->Front().Normalized();

	if (localForward.Equals(camDirection, 0.05f)) 
	{
		rotationAchieved = true;
	}
	else 
	{
		targetRotation = Quat::RotateFromTo(localForward, camDirection);
		targetRotation.Normalize();
	}

	float3 endposition = boundingSphere.pos - (camDirection * camDistance);

	float deltaTime = App->GetDeltaTime();

	if (!positionAchieved)
	{
		//Position proportional
		float3 positionError = endposition - position;
		if (positionError.Equals(float3::zero, 0.05f)) 
		{
			positionAchieved = true;
		}
		else 
		{
			float3 velocityPosition = positionError * KpPosition;
			float3 nextPos = position + velocityPosition * deltaTime;
			SetPosition(nextPos);
		}
		
	}


	if (!rotationAchieved)
	{
		//Rotation proportional
		Quat rotationError = targetRotation * rotation.Normalized().Inverted();
		rotationError.Normalize();

		if (rotationError.Equals(Quat::identity, 0.05f)) 
		{
			rotationAchieved = true;
		}
		else 
		{
			float3 axis;
			float angle;
			rotationError.ToAxisAngle(axis, angle);
			axis.Normalize();

			float3 velocityRotation = axis * angle * KpRotation;
			Quat angularVelocityQuat(velocityRotation.x, velocityRotation.y, velocityRotation.z, 0.0f);
			Quat wq_0 = angularVelocityQuat * rotation;


			float deltaValue = 0.5f * deltaTime;
			Quat deltaRotation = Quat(deltaValue * wq_0.x, deltaValue * wq_0.y, deltaValue * wq_0.z, deltaValue * wq_0.w);

			Quat nextRotation(rotation.x + deltaRotation.x,
				rotation.y + deltaRotation.y,
				rotation.z + deltaRotation.z,
				rotation.w + deltaRotation.w);
			nextRotation.Normalize();

			ApplyRotationWithFixedUp(nextRotation, float3::unitY);

		}
		
	}

	if (positionAchieved && rotationAchieved) 
	{
		isFocusing = false;
	}

}
void CameraEngine::Focus(GameObject* gameObject)
{
	Component* transform = gameObject->GetComponent(ComponentType::TRANSFORM);
	if (transform)
	{
		std::list<GameObject*> insideGameObjects = gameObject->GetGameObjectsInside();
		AABB minimalAABB;
		std::vector<math::vec> outputArray{};
		for (GameObject* object : insideGameObjects)
		{
			if (object)
			{
				ComponentTransform* transform =
					static_cast<ComponentTransform*>(object->GetComponent(ComponentType::TRANSFORM));
				outputArray.push_back(transform->GetEncapsuledAABB().minPoint);
				outputArray.push_back(transform->GetEncapsuledAABB().maxPoint);
			}
		}
		minimalAABB = minimalAABB.MinimalEnclosingAABB(outputArray.data(), (int)outputArray.size());

		if (isUsingProportionalController) 
		{
			FocusProportional(minimalAABB);
		}
		else 
		{
			FocusInterpolated(minimalAABB);
		}
		
	}
}

void CameraEngine::Orbit(const OBB& obb)
{

	FreeLook();

	float3 posToOrbit = obb.CenterPoint();

	vec camDirection = frustum->Front().Normalized();
	float camDistance = posToOrbit.Distance(frustum->Pos());

	float3 newPosition = posToOrbit - (camDirection * camDistance);

	SetPosition(newPosition);
}

void CameraEngine::UnlimitedCursor()
{
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	if (mouseWarped)
	{
		App->GetModule<ModuleInput>()->SetMouseMotionX(float(mouseX - lastMouseX));
		App->GetModule<ModuleInput>()->SetMouseMotionY(float(mouseY - lastMouseY));
		mouseWarped = false;
	}
	int width, height;
	SDL_GetWindowSize(App->GetModule<ModuleWindow>()->GetWindow(), &width, &height);
	if (mouseX <= 0)
	{
		lastMouseX = width - 1;
		lastMouseY = mouseY;
		SDL_WarpMouseInWindow(App->GetModule<ModuleWindow>()->GetWindow(), width - 1, mouseY);
		mouseWarped = true;
	}
	if (mouseX >= width - 1)
	{
		lastMouseX = 0;
		lastMouseY = mouseY;
		SDL_WarpMouseInWindow(App->GetModule<ModuleWindow>()->GetWindow(), 0, mouseY);
		mouseWarped = true;
	}
	if (mouseY <= 0)
	{
		lastMouseX = mouseX;
		lastMouseY = height - 1;
		SDL_WarpMouseInWindow(App->GetModule<ModuleWindow>()->GetWindow(), mouseX, height - 1);
		mouseWarped = true;
	}
	if (mouseY >= height - 1)
	{
		lastMouseX = mouseX;
		lastMouseY = 0;
		SDL_WarpMouseInWindow(App->GetModule<ModuleWindow>()->GetWindow(), mouseX, 0);
		mouseWarped = true;
	}
}
