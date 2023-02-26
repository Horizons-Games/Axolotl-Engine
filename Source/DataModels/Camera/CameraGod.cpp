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

bool CameraGod::Update()
{
	projectionMatrix = frustum->ProjectionMatrix();
	viewMatrix = frustum->ViewMatrix();

	//Shift speed
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) != KeyState::IDLE)
	{
		Run();
	}
	else
	{
		Walk();
	}

	//Move and rotate with right buttons and ASDWQE
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	App->input->SetMouseMotionX(mouseX - lastMouseX);
	App->input->SetMouseMotionY(mouseY - lastMouseY);

	int width, height;
	SDL_GetWindowSize(App->window->GetWindow(), &width, &height);
	SDL_WarpMouseInWindow(App->window->GetWindow(), width / 2, height / 2);

	lastMouseX = width / 2;
	lastMouseY = height / 2;

	Move();
	FreeLook();
	

	KeyboardRotate();

	if (frustumMode == EFrustumMode::offsetFrustum)
	{
		RecalculateOffsetPlanes();
	}

	return true;
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