#include "CameraGod.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"

CameraGod::CameraGod()
	: Camera(CameraType::C_GOD)
{
}

CameraGod::CameraGod(const std::unique_ptr<Camera>& camera)
	: Camera(camera,CameraType::C_GOD)
{
}

CameraGod::~CameraGod()
{
}

bool CameraGod::Update()
{
	if (App->input->GetInFocus())
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
		if (!App->IsDebuggingGame())
		{
			keepMouseCentered();
			Move();
			FreeLook();
		}

		KeyboardRotate();

		if (frustumMode == EFrustumMode::offsetFrustum)
		{
			RecalculateOffsetPlanes();
		}
	}

	return true;
}

void CameraGod::Move()
{
	float deltaTime = App->GetDeltaTime();

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
}

void CameraGod::keepMouseCentered()
{
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	App->input->SetMouseMotionX(float(mouseX - lastMouseX));
	App->input->SetMouseMotionY(float(mouseY - lastMouseY));

	int width, height;
	SDL_GetWindowSize(App->window->GetWindow(), &width, &height);
	SDL_WarpMouseInWindow(App->window->GetWindow(), width / 2, height / 2);

	lastMouseX = width / 2;
	lastMouseY = height / 2;
}