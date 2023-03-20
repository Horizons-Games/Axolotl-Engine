#include "CameraGameObject.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"

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
	if (App->input->GetInFocus())
	{
		projectionMatrix = frustum->ProjectionMatrix();
		viewMatrix = frustum->ViewMatrix();

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
	return true;
}