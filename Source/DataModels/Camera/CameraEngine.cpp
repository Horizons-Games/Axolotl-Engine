#include "CameraEngine.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"

#include "Scene/Scene.h"

#include "GameObject/GameObject.h"

#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentTransform.h"

#include "Resources/ResourceMesh.h"

#include "Windows/EditorWindows/WindowScene.h"

#include "Geometry/Sphere.h"
#include "Geometry/Triangle.h"
#include "Math/Quat.h"
#include "Math/float3x3.h"
#include "Physics/Physics.h"

CameraEngine::CameraEngine() : Camera(CameraType::C_ENGINE){};

CameraEngine::CameraEngine(const std::unique_ptr<Camera>& camera) : Camera(camera, CameraType::C_ENGINE)
{
}

CameraEngine::~CameraEngine(){};

bool CameraEngine::Update()
{
	projectionMatrix = frustum->ProjectionMatrix();
	viewMatrix = frustum->ViewMatrix();

	App->input->SetDefaultCursor();

	bool sceneFocused = App->editor->IsSceneFocused();

	if (sceneFocused)
	{
		// We block everything on while Focus (slerp) to avoid camera problems
		if (isFocusing)
		{
			if (focusFlag)
				Focus(App->scene->GetSelectedGameObject());
		}
		else
		{
			// Shift speed
			if (App->input->GetKey(SDL_SCANCODE_LSHIFT) != KeyState::IDLE)
				Run();
			else
				Walk();

			// this should probably be encapsulated in a method, or moved to the Physics part of the Engine
			// --RAYCAST CALCULATION-- //
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::DOWN &&
				App->input->GetKey(SDL_SCANCODE_LALT) == KeyState::IDLE)
			{
				LineSegment ray;
				if (Physics::ScreenPointToRay(App->input->GetMousePosition(), ray))
				{
					RaycastHit hit;
					if (Physics::Raycast(ray, hit))
					{
						SetNewSelectedGameObject(hit.gameObject);
					}
				}
			}
			// --RAYCAST CALCULATION-- //

			// Move and rotate with right buttons and ASDWQE
			if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) != KeyState::IDLE &&
				App->input->GetKey(SDL_SCANCODE_LALT) == KeyState::IDLE)
			{
				focusFlag = false;
				App->input->SetFreeLookCursor();
				UnlimitedCursor();
				Move();
				FreeLook();
			}

			// Zoom with mouse wheel
			if (App->input->IsMouseWheelScrolled() && App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::IDLE)
			{
				focusFlag = false;
				Zoom();
			}

			// Move camera UP/DOWN and RIGHT/LEFT with mouse mid button
			if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) != KeyState::IDLE)
			{
				focusFlag = false;
				App->input->SetMoveCursor();
				UnlimitedCursor();
				Move();
			}

			// Focus
			if (App->scene->GetSelectedGameObject() != App->scene->GetLoadedScene()->GetRoot() &&
				App->input->GetKey(SDL_SCANCODE_F) != KeyState::IDLE)
			{
				focusFlag = true;
				isFocusing = true;
			}

			// Orbit object with ALT + LEFT MOUSE CLICK
			if (App->scene->GetSelectedGameObject() != App->scene->GetLoadedScene()->GetRoot() &&
				App->input->GetKey(SDL_SCANCODE_LALT) != KeyState::IDLE &&
				App->input->GetMouseButton(SDL_BUTTON_LEFT) != KeyState::IDLE)
			{
				const OBB& obb = App->scene->GetSelectedGameObject()->GetObjectOBB();
				focusFlag = false;
				App->input->SetOrbitCursor();
				UnlimitedCursor();
				Orbit(obb);
			}

			// Zoom with ALT + RIGHT MOUSE CLICK
			if (App->input->GetKey(SDL_SCANCODE_LALT) != KeyState::IDLE &&
				App->input->GetMouseButton(SDL_BUTTON_RIGHT) != KeyState::IDLE &&
				App->input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::IDLE) // Not pressing mouse left button
			{
				App->input->SetZoomCursor();
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
	// Increase/decrease camera velocity with mouse wheel
	if (App->input->IsMouseWheelScrolled() && App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KeyState::IDLE)
	{
		moveSpeed += App->input->GetMouseWheel().y;
		if (moveSpeed < 1.0f)
		{
			moveSpeed = 1.0f;
		}
		if (moveSpeed > 900.0f)
		{
			moveSpeed = 900.0f;
		}
	}

	// Forward
	if (App->input->GetKey(SDL_SCANCODE_W) != KeyState::IDLE)
	{
		position += frustum->Front().Normalized() * moveSpeed * acceleration * App->GetDeltaTime();
		SetPosition(position);
	}

	// Backward
	if (App->input->GetKey(SDL_SCANCODE_S) != KeyState::IDLE)
	{
		position += -(frustum->Front().Normalized()) * moveSpeed * acceleration * App->GetDeltaTime();
		SetPosition(position);
	}

	// Left
	if (App->input->GetKey(SDL_SCANCODE_A) != KeyState::IDLE)
	{
		position += -(frustum->WorldRight()) * moveSpeed * acceleration * App->GetDeltaTime();
		SetPosition(position);
	}

	// Right
	if (App->input->GetKey(SDL_SCANCODE_D) != KeyState::IDLE)
	{
		position += frustum->WorldRight() * moveSpeed * acceleration * App->GetDeltaTime();
		SetPosition(position);
	}

	// Up
	if (App->input->GetKey(SDL_SCANCODE_E) != KeyState::IDLE)
	{
		position += frustum->Up() * moveSpeed * acceleration * App->GetDeltaTime();
		SetPosition(position);
	}

	// Down
	if (App->input->GetKey(SDL_SCANCODE_Q) != KeyState::IDLE)
	{
		position += -(frustum->Up()) * moveSpeed * acceleration * App->GetDeltaTime();
		SetPosition(position);
	}

	// Move UP/DOWN and RIGHT/LEFT with mid mouse button
	if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) != KeyState::IDLE)
	{
		float mouseSpeedPercentage = 0.05f;
		float xrel = -App->input->GetMouseMotion().x * (rotationSpeed * mouseSpeedPercentage) * App->GetDeltaTime();
		float yrel = App->input->GetMouseMotion().y * (rotationSpeed * mouseSpeedPercentage) * App->GetDeltaTime();

		position += (frustum->WorldRight()) * xrel;
		position += (frustum->Up()) * yrel;
		SetPosition(position);
	}
}

void CameraEngine::Zoom()
{
	if (App->input->IsMouseWheelScrolled())
	{
		float zoomSpeed = App->input->GetMouseWheel().y * DEFAULT_MOUSE_ZOOM_SPEED;

		position += frustum->Front().Normalized() * zoomSpeed * App->GetDeltaTime();
	}
	else
	{
		float zoomSpeed = App->input->GetMouseMotion().x * DEFAULT_MOUSE_ZOOM_SPEED;

		position += frustum->Front().Normalized() * zoomSpeed * App->GetDeltaTime();
	}
	SetPosition(position);
}

void CameraEngine::Focus(const OBB& obb)
{
	Sphere boundingSphere = obb.MinimalEnclosingSphere();

	float radius = boundingSphere.r;
	if (boundingSphere.r < 1.f)
		radius = 1.f;
	float fov = frustum->HorizontalFov();
	float camDistance = radius / float(sin(fov / 2.0));
	vec camDirection = (boundingSphere.pos - frustum->Pos()).Normalized();

	float3 endposition = boundingSphere.pos - (camDirection * camDistance);

	bool isSamePosition = false;
	if (position.Equals(endposition))
	{
		isSamePosition = true;
	}
	else
	{
		position = position.Lerp(endposition, App->GetDeltaTime() * rotationSpeed * 2);
		SetPosition(position);
	}

	bool isSameRotation = false;
	SetLookAt(boundingSphere.pos, isSameRotation);

	if (isSamePosition && isSameRotation)
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
				outputArray.push_back(object->GetEncapsuledAABB().minPoint);
				outputArray.push_back(object->GetEncapsuledAABB().maxPoint);
			}
		}
		minimalAABB = minimalAABB.MinimalEnclosingAABB(outputArray.data(), (int) outputArray.size());

		Focus(minimalAABB);
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
		App->input->SetMouseMotionX(float(mouseX - lastMouseX));
		App->input->SetMouseMotionY(float(mouseY - lastMouseY));
		mouseWarped = false;
	}
	int width, height;
	SDL_GetWindowSize(App->window->GetWindow(), &width, &height);
	if (mouseX <= 0)
	{
		lastMouseX = width - 1;
		lastMouseY = mouseY;
		SDL_WarpMouseInWindow(App->window->GetWindow(), width - 1, mouseY);
		mouseWarped = true;
	}
	if (mouseX >= width - 1)
	{
		lastMouseX = 0;
		lastMouseY = mouseY;
		SDL_WarpMouseInWindow(App->window->GetWindow(), 0, mouseY);
		mouseWarped = true;
	}
	if (mouseY <= 0)
	{
		lastMouseX = mouseX;
		lastMouseY = height - 1;
		SDL_WarpMouseInWindow(App->window->GetWindow(), mouseX, height - 1);
		mouseWarped = true;
	}
	if (mouseY >= height - 1)
	{
		lastMouseX = mouseX;
		lastMouseY = 0;
		SDL_WarpMouseInWindow(App->window->GetWindow(), mouseX, 0);
		mouseWarped = true;
	}
}
