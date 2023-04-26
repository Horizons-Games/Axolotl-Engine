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