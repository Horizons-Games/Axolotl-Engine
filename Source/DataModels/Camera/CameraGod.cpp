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
	if (App->GetModule<ModuleInput>()->GetInFocus())
	{
		projectionMatrix = frustum->ProjectionMatrix();
		viewMatrix = frustum->ViewMatrix();

		//Shift speed
		if (App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_LSHIFT) != KeyState::IDLE)
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
			KeepMouseCentered();
			Move();
			if (!backFromDebugging)
			{
				FreeLook();
			}
			backFromDebugging = false;
		}
		else
		{
			backFromDebugging = true;
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
	if (App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_W) != KeyState::IDLE)
	{
		position += frustum->Front().Normalized() *
			moveSpeed * acceleration * deltaTime;
		SetPosition(position);
	}

	//Backward
	if (App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_S) != KeyState::IDLE)
	{
		position += -(frustum->Front().Normalized()) *
			moveSpeed * acceleration * deltaTime;
		SetPosition(position);
	}

	//Left
	if (App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_A) != KeyState::IDLE)
	{
		position += -(frustum->WorldRight()) * moveSpeed * acceleration * deltaTime;
		SetPosition(position);
	}

	//Right
	if (App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_D) != KeyState::IDLE)
	{
		position += frustum->WorldRight() * moveSpeed * acceleration * deltaTime;
		SetPosition(position);
	}

	//Up
	if (App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_E) != KeyState::IDLE)
	{
		position += frustum->Up() * moveSpeed * acceleration * deltaTime;
		SetPosition(position);
	}

	//Down
	if (App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_Q) != KeyState::IDLE)
	{
		position += -(frustum->Up()) * moveSpeed * acceleration * deltaTime;
		SetPosition(position);
	}
}

void CameraGod::KeepMouseCentered()
{
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	App->GetModule<ModuleInput>()->SetMouseMotionX(float(mouseX - lastMouseX));
	App->GetModule<ModuleInput>()->SetMouseMotionY(float(mouseY - lastMouseY));

	int width, height;
	SDL_GetWindowSize(App->GetModule<ModuleWindow>()->GetWindow(), &width, &height);
	SDL_WarpMouseInWindow(App->GetModule<ModuleWindow>()->GetWindow(), width / 2, height / 2);	

	lastMouseX = width / 2;
	lastMouseY = height / 2;
}