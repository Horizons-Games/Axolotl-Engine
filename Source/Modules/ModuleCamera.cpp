#pragma warning (disable: 26495)

#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModulePlayer.h"


#include "Scene/Scene.h"

#include "GameObject/GameObject.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentCamera.h"

#include "Resources/ResourceMesh.h"

#include "Windows/EditorWindows/WindowScene.h"

#include "Math/float3x3.h"
#include "Math/Quat.h"
#include "Geometry/Sphere.h"
#include "Geometry/Triangle.h"

#ifdef ENGINE
#include "Camera/CameraEngine.h"
#endif // ENGINE
#include "Camera/CameraGod.h"
#include "Camera/CameraGameObject.h"



ModuleCamera::ModuleCamera() {};

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

	RecalculateOrthoProjectionMatrix();
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
		if (App->IsOnPlayMode())
		{
			selectedCamera = App->player->GetCameraPlayer();
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
		std::vector<ComponentCamera*> loadedCameras = App->scene->GetLoadedScene()->GetSceneCameras();
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
	std::pair<int, int> region = App->editor->GetAvailableRegion();
	orthoProjectionMatrix = float4x4::D3DOrthoProjLH(-1, 1, region.first, region.second);
}