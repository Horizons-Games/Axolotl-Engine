#pragma warning (disable: 26495)

#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#ifndef ENGINE
#include "ModulePlayer.h"
#endif // !ENGINE

#include "Scene/Scene.h"

#include "Components/ComponentCamera.h"

#ifdef ENGINE
#include "Camera/CameraEngine.h"
#endif // ENGINE
#include "Camera/CameraGod.h"
#include "Camera/CameraGameObject.h"

ModuleCamera::ModuleCamera() {
};

ModuleCamera::~ModuleCamera() {
};

bool ModuleCamera::Init()
{
	#ifdef ENGINE
		camera = std::make_unique <CameraEngine>();
	#else // ENGINE
		camera = std::make_unique <CameraGod>();
	#endif // GAMEMODE

	selectedPosition = 0;
	camera->Init();
	return true;
}

bool ModuleCamera::Start()
{
	camera->Start();
	#ifdef ENGINE
		selectedCamera = camera.get();
	#else // ENGINE
		//selectedPosition = 1;
		//SetSelectedCamera(selectedPosition);
		//if (selectedCamera == nullptr)
		//{
			selectedPosition = 0;
			selectedCamera = camera.get();
		//}
	#endif // GAMEMODE
	return true;
}

update_status ModuleCamera::Update()
{
	if (
#ifdef ENGINE
		App->editor->IsSceneFocused()
#else // ENGINE
		true
#endif // GAMEMODE
		)
	{
		if (App->input->GetKey(SDL_SCANCODE_1) == KeyState::DOWN)
		{
			selectedPosition--;
			SetSelectedCamera(selectedPosition);
		}
		else if (App->input->GetKey(SDL_SCANCODE_2) == KeyState::DOWN)
		{
			selectedPosition++;
			SetSelectedCamera(selectedPosition);
		}
		else if (App->input->GetKey(SDL_SCANCODE_3) == KeyState::DOWN)
		{
			selectedPosition = 0;
			SetSelectedCamera(selectedPosition);
		}
		else if (App->input->GetKey(SDL_SCANCODE_4) == KeyState::DOWN)
		{
			selectedPosition = 1;
			SetSelectedCamera(selectedPosition);
		}
	}

	selectedCamera->Update();

	return update_status::UPDATE_CONTINUE;
}


void ModuleCamera::ChangeCamera(CameraType newType)
{
	switch (newType)
	{
	case CameraType::C_GOD:
		camera = std::make_unique <CameraGod>(std::move(camera));
		break;
#ifdef ENGINE
	case CameraType::C_ENGINE:
		camera = std::make_unique <CameraEngine>(std::move(camera));
		break;
#endif // ENGINE

	}
	
}

void ModuleCamera::SetSelectedCamera(int cameraNumber)
{
	if (cameraNumber <= 0)
	{
#ifdef ENGINE
		selectedPosition = 0;
		selectedCamera = camera.get();
#else
		selectedPosition = 0;
		selectedCamera = App->player->GetCameraPlayer();
		if (!selectedCamera)
		{
			selectedPosition = 1;
			selectedCamera = camera.get();
		}
		else
		{
			camera->SetPosition(selectedCamera->GetPosition());
			camera->GetFrustum()->SetFront(selectedCamera->GetFrustum()->Front());
			camera->GetFrustum()->SetUp(selectedCamera->GetFrustum()->Up());
		}
#endif // ENGINE
	}
#ifndef ENGINE
	else if (cameraNumber == 1)
	{
		selectedPosition = 1;
		selectedCamera = camera.get();
	}
#endif // !ENGINE
	else
	{
		std::vector<GameObject*> loadedCameras = App->scene->GetLoadedScene()->GetSceneCameras();
		if (loadedCameras.size() >= cameraNumber)
		{
#ifdef ENGINE
			selectedCamera = (static_cast<ComponentCamera*>(loadedCameras
				[cameraNumber - 1]->GetComponent(ComponentType::CAMERA)))->GetCamera();
#else
			selectedCamera = (static_cast<ComponentCamera*>(loadedCameras
				[cameraNumber - 2]->GetComponent(ComponentType::CAMERA)))->GetCamera();
#endif
			selectedPosition = cameraNumber;
			camera->SetPosition(selectedCamera->GetPosition());
			camera->GetFrustum()->SetFront(selectedCamera->GetFrustum()->Front());
			camera->GetFrustum()->SetUp(selectedCamera->GetFrustum()->Up());
		}
		else
		{
			selectedPosition--;
		}
	}
}

Camera* ModuleCamera::GetCamera()
{
	return selectedCamera;
}