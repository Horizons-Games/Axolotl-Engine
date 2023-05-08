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

	if (App->GetModule<ModuleInput>()->GetInFocus())
	{
		if (!App->GetModule<ModulePlayer>()->IsStatic() && !SDL_ShowCursor(SDL_QUERY))
		{
			UpdateCameraWithMousePos();
		}
	}
	return true;
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