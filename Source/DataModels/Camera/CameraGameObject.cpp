#include "CameraGameObject.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModulePlayer.h"

CameraGameObject::CameraGameObject()
	: Camera(CameraType::C_GAMEOBJECT)
{
};

CameraGameObject::CameraGameObject(const std::unique_ptr<Camera>& camera)
	: Camera(camera, CameraType::C_GAMEOBJECT)
{
}

CameraGameObject::~CameraGameObject()
{
};


bool CameraGameObject::Update()
{
	projectionMatrix = frustum->ProjectionMatrix();
	viewMatrix = frustum->ViewMatrix();

	if (App->input->GetInFocus())
	{
#ifdef ENGINE
		UpdateCameraWithMousePos();
#else // ENGINE
		if (!App->player->IsStatic())
		{
			UpdateCameraWithMousePos();
		}
#endif
	}
	return true;
}

void CameraGameObject::UpdateCameraWithMousePos()
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