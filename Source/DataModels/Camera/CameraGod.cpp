#include "CameraGod.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"

CameraGod::CameraGod()
	: Camera(CameraType::C_GOD)
{
}

CameraGod::~CameraGod()
{
}

bool CameraGod::Init()
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

bool CameraGod::Start()
{
	return true;
}

bool CameraGod::Update()
{
	return false;
}

void CameraGod::Move()
{
	float deltaTime = App->GetDeltaTime();

	//Increase/decrease camera velocity with mouse wheel
	if (App->input->IsMouseWheelScrolled() && App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KeyState::IDLE)
	{
		moveSpeed += App->input->GetMouseWheel().y;
		if (moveSpeed < 1.0f)
			moveSpeed = 1.0f;
		if (moveSpeed > 900.0f)
			moveSpeed = 900.0f;
	}

	//Forward
	if (App->input->GetKey(SDL_SCANCODE_W) != KeyState::IDLE)
	{
		position += frustum->Front().Normalized() *
			moveSpeed * acceleration * deltaTime;
		SetPosition(position);
	}

	//Backward
	if (App->input->GetKey(SDL_SCANCODE_S) != KeyState::IDLE)
	{
		position += -(frustum->Front().Normalized()) *
			moveSpeed * acceleration * deltaTime;
		SetPosition(position);
	}

	//Left
	if (App->input->GetKey(SDL_SCANCODE_A) != KeyState::IDLE)
	{
		position += -(frustum->WorldRight()) * moveSpeed * acceleration * deltaTime;
		SetPosition(position);
	}

	//Right
	if (App->input->GetKey(SDL_SCANCODE_D) != KeyState::IDLE)
	{
		position += frustum->WorldRight() * moveSpeed * acceleration * deltaTime;
		SetPosition(position);
	}

	//Up
	if (App->input->GetKey(SDL_SCANCODE_E) != KeyState::IDLE)
	{
		position += frustum->Up() * moveSpeed * acceleration * deltaTime;
		SetPosition(position);
	}

	//Down
	if (App->input->GetKey(SDL_SCANCODE_Q) != KeyState::IDLE)
	{
		position += -(frustum->Up()) * moveSpeed * acceleration * deltaTime;
		SetPosition(position);
	}

	//Move UP/DOWN and RIGHT/LEFT with mid mouse button
	if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) != KeyState::IDLE)
	{
		float deltaTime = App->GetDeltaTime();
		float mouseSpeedPercentage = 0.05f;
		float xrel = -App->input->GetMouseMotion().x * (rotationSpeed * mouseSpeedPercentage) * deltaTime;
		float yrel = App->input->GetMouseMotion().y * (rotationSpeed * mouseSpeedPercentage) * deltaTime;

		position += (frustum->WorldRight()) * xrel;
		position += (frustum->Up()) * yrel;
		SetPosition(position);
	}
}
