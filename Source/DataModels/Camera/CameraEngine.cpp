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
#include "Components/ComponentBoundingBoxes.h"
#include "Components/ComponentMeshRenderer.h"

#include "Resources/ResourceMesh.h"

#include "Windows/EditorWindows/WindowScene.h"

#include "Math/float3x3.h"
#include "Math/Quat.h"
#include "Geometry/Sphere.h"
#include "Geometry/Triangle.h"

CameraEngine::CameraEngine() 
: Camera(CameraType::C_ENGINE)
{
};

CameraEngine::~CameraEngine()
{
	delete frustum;
};

bool CameraEngine::Init()
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

	if (frustumMode == offsetFrustum) RecalculateOffsetPlanes();

	return true;
}

bool CameraEngine::Start()
{
	return true;
}

bool CameraEngine::Update()
{

	projectionMatrix = frustum->ProjectionMatrix();
	viewMatrix = frustum->ViewMatrix();

	if (App->editor->IsSceneFocused())
	{
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) != KeyState::IDLE)
			Run();
		else
			Walk();

		// --RAYCAST CALCULATION-- //
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) != KeyState::IDLE
			&& App->input->GetKey(SDL_SCANCODE_LALT) == KeyState::IDLE)
		{
			const WindowScene* windowScene = App->editor->GetScene();
			LineSegment ray = CreateRaycastFromMousePosition(windowScene);
			CalculateHittedGameObjects(ray);
		}
		// --RAYCAST CALCULATION-- //

		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) != KeyState::IDLE)
		{
			Move();
			FreeLook();
		}

		if (App->input->IsMouseWheelScrolled())
		{
			Zoom();
		}

		if (App->scene->GetSelectedGameObject() != App->scene->GetLoadedScene()->GetRoot() &&
			App->input->GetKey(SDL_SCANCODE_F) != KeyState::IDLE)
			Focus(App->scene->GetSelectedGameObject());

		if (App->scene->GetSelectedGameObject() != App->scene->GetLoadedScene()->GetRoot() &&
			App->input->GetKey(SDL_SCANCODE_LALT) != KeyState::IDLE &&
			App->input->GetMouseButton(SDL_BUTTON_LEFT) != KeyState::IDLE)
		{
			const OBB& obb = static_cast<ComponentBoundingBoxes*>(
				App->scene->GetSelectedGameObject()->GetComponent(ComponentType::BOUNDINGBOX))->GetObjectOBB();

			SetLookAt(obb.CenterPoint());
			Orbit(obb);
		}

		KeyboardRotate();
		if (frustumMode == offsetFrustum) RecalculateOffsetPlanes();
	}

	return true;
}

void CameraEngine::Move()
{
	float deltaTime = App->GetDeltaTime();

	if (App->input->GetKey(SDL_SCANCODE_W) != KeyState::IDLE)
	{
		position = position + frustum->Front().Normalized() *
			moveSpeed * acceleration * deltaTime;
		frustum->SetPos(position);
	}

	if (App->input->GetKey(SDL_SCANCODE_S) != KeyState::IDLE)
	{
		position = position + -(frustum->Front().Normalized()) *
			moveSpeed * acceleration * deltaTime;
		frustum->SetPos(position);
	}

	if (App->input->GetKey(SDL_SCANCODE_A) != KeyState::IDLE)
	{
		position = position + -(frustum->WorldRight()) * moveSpeed * acceleration * deltaTime;
		frustum->SetPos(position);
	}

	if (App->input->GetKey(SDL_SCANCODE_D) != KeyState::IDLE)
	{
		position = position + frustum->WorldRight() * moveSpeed * acceleration * deltaTime;
		frustum->SetPos(position);
	}

	if (App->input->GetKey(SDL_SCANCODE_E) != KeyState::IDLE)
	{
		position = position + frustum->Up() * moveSpeed * acceleration * deltaTime;
		frustum->SetPos(position);
	}

	if (App->input->GetKey(SDL_SCANCODE_Q) != KeyState::IDLE)
	{
		position = position + -(frustum->Up()) * moveSpeed * acceleration * deltaTime;
		frustum->SetPos(position);
	}
}

void CameraEngine::KeyboardRotate()
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


void CameraEngine::FreeLook()
{
	float yaw = 0.f, pitch = 0.f;
	float xrel = App->input->GetMouseMotion().x;
	float yrel = App->input->GetMouseMotion().y;
	float rotationAngle = RadToDeg(frustum->Front().Normalized().AngleBetween(float3::unitY));

	if (abs(xrel) + abs(yrel) != 0 && mouseSpeedModifier < MAX_MOUSE_SPEED_MODIFIER)
	{
		mouseSpeedModifier += 0.5f;
	}
	else if (abs(xrel) + abs(yrel) == 0)
		mouseSpeedModifier = DEFAULT_MOUSE_SPEED_MODIFIER;

	if (yrel > 0) { if (rotationAngle - yrel > 0) pitch = math::DegToRad(yrel); }
	else if (yrel < 0) { if (rotationAngle - yrel < 180) pitch = math::DegToRad(yrel); }

	yaw = math::DegToRad(-xrel);

	float deltaTime = App->GetDeltaTime();
	Quat pitchQuat(frustum->WorldRight(), pitch * mouseSpeedModifier * deltaTime);
	Quat yawQuat(float3::unitY, yaw * mouseSpeedModifier * deltaTime);

	float3x3 rotationMatrixX = float3x3::FromQuat(pitchQuat);
	float3x3 rotationMatrixY = float3x3::FromQuat(yawQuat);
	float3x3 rotationDeltaMatrix = rotationMatrixY * rotationMatrixX;

	ApplyRotation(rotationDeltaMatrix);
}

void CameraEngine::Run()
{
	acceleration = DEFAULT_SHIFT_ACCELERATION;
}

void CameraEngine::Walk()
{
	acceleration = 1.f;
}

void CameraEngine::Zoom()
{
	float newHFOV = GetHFOV() - App->input->GetMouseWheel().y;

	if (newHFOV <= MAX_HFOV && newHFOV >= MIN_HFOV)
		SetHFOV(math::DegToRad(newHFOV));
}

void CameraEngine::Focus(const OBB& obb)
{
	math::Sphere minSphere = obb.MinimalEnclosingSphere();

	float3 point = minSphere.Centroid();

	position = point - frustum->Front().Normalized() * minSphere.Diameter();

	SetLookAt(point);

	frustum->SetPos(position);
}

void CameraEngine::Focus(GameObject* gameObject)
{
	std::list<GameObject*> insideGameObjects = gameObject->GetGameObjectsInside();
	AABB minimalAABB;
	std::vector<math::vec> outputArray{};
	for (GameObject* object : insideGameObjects)
	{
		if (object)
		{
			ComponentBoundingBoxes* boundingBox =
				static_cast<ComponentBoundingBoxes*>(object->GetComponent(ComponentType::BOUNDINGBOX));
			outputArray.push_back(boundingBox->GetEncapsuledAABB().minPoint);
			outputArray.push_back(boundingBox->GetEncapsuledAABB().maxPoint);
		}
	}
	minimalAABB = minimalAABB.MinimalEnclosingAABB(outputArray.data(), (int)outputArray.size());
	math::Sphere minSphere = minimalAABB.MinimalEnclosingSphere();;

	if (minSphere.r == 0) minSphere.r = 1.f;
	float3 point = minSphere.Centroid();
	position = point - frustum->Front().Normalized() * minSphere.Diameter();

	SetLookAt(point);

	frustum->SetPos(position);
}


void CameraEngine::Orbit(const OBB& obb)
{
	float distance = obb.CenterPoint().Distance(position);

	vec oldPosition = position + frustum->Front().Normalized() * distance;

	float deltaTime = App->GetDeltaTime();
	Quat verticalOrbit(
		frustum->WorldRight(),
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

	vec newPosition = position + frustum->Front().Normalized() * distance;

	position = position + (oldPosition - newPosition);

	frustum->SetPos(position);
}