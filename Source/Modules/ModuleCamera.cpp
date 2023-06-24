#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "ModulePlayer.h"
#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "Components/ComponentCamera.h"

#ifdef ENGINE
	#include "Camera/CameraEngine.h"
#endif // ENGINE
#include "Camera/CameraGameObject.h"
#include "Camera/CameraGod.h"

ModuleCamera::ModuleCamera(){};

ModuleCamera::~ModuleCamera(){};

bool ModuleCamera::Init()
{
#ifdef ENGINE
	camera = std::make_unique<CameraEngine>();
#else  // ENGINE
	camera = std::make_unique<CameraGod>();
#endif // GAMEMODE

	selectedPosition = 0;
	return camera->Init();
}

bool ModuleCamera::Start()
{
	if (!camera->Start())
	{
		return false;
	}
#ifdef ENGINE
	selectedCamera = camera.get();
#else  // ENGINE
	// selectedPosition = 1;
	// SetSelectedCamera(selectedPosition);
	// if (selectedCamera == nullptr)
	//{
	selectedPosition = 0;
	selectedCamera = camera.get();
	//}
#endif // GAMEMODE

	RecalculateOrthoProjectionMatrix();
	return true;
}

update_status ModuleCamera::Update()
{
	ModuleInput* input = App->GetModule<ModuleInput>();
	if (
#ifdef ENGINE
		App->GetModule<ModuleEditor>()->IsSceneFocused()
#else  // ENGINE
		true
#endif // GAMEMODE
	)
	{
		if (input->GetKey(SDL_SCANCODE_1) == KeyState::DOWN)
		{
			selectedPosition--;
			SetSelectedCamera(selectedPosition);
		}
		else if (input->GetKey(SDL_SCANCODE_2) == KeyState::DOWN)
		{
			selectedPosition++;
			SetSelectedCamera(selectedPosition);
		}
		else if (input->GetKey(SDL_SCANCODE_3) == KeyState::DOWN)
		{
			selectedPosition = 0;
			SetSelectedCamera(selectedPosition);
		}
		else if (input->GetKey(SDL_SCANCODE_4) == KeyState::DOWN)
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
			camera = std::make_unique<CameraGod>(std::move(camera));
			break;
#ifdef ENGINE
		case CameraType::C_ENGINE:
			camera = std::make_unique<CameraEngine>(std::move(camera));
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
		if (App->IsOnPlayMode())
		{
			selectedCamera = App->GetModule<ModulePlayer>()->GetCameraPlayer();
			if (!selectedCamera)
			{
				selectedPosition = 1;
				selectedCamera = camera.get();
			}
		}
		else
		{
			selectedCamera = camera.get();
		}
#else
		selectedPosition = 0;
		selectedCamera = App->GetModule<ModulePlayer>()->GetCameraPlayer();
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
		std::vector<ComponentCamera*> loadedCameras =
			App->GetModule<ModuleScene>()->GetLoadedScene()->GetSceneCameras();
		if (loadedCameras.size() >= cameraNumber)
		{
#ifdef ENGINE
			selectedCamera = loadedCameras[cameraNumber - 1l]->GetCamera();
#else
			selectedCamera = loadedCameras[cameraNumber - 2l]->GetCamera();
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

void ModuleCamera::RecalculateOrthoProjectionMatrix()
{
	std::pair<float, float> region = App->GetModule<ModuleEditor>()->GetAvailableRegion();
	orthoProjectionMatrix = float4x4::D3DOrthoProjLH(-1, 1, floor(region.first), floor(region.second));
}