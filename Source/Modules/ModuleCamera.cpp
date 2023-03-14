#pragma warning (disable: 26495)

#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"

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

	camera->Init();
	return true;
}

bool ModuleCamera::Start()
{
	camera->Start();
	selectedCamera = camera.get();
	return true;
}

update_status ModuleCamera::Update()
{
	if (App->input->GetKey(SDL_SCANCODE_0) == KeyState::DOWN)
	{
		SetSelectedCamera(0);
	}
	else if (App->input->GetKey(SDL_SCANCODE_1) == KeyState::DOWN)
	{
		SetSelectedCamera(1);
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
	if (cameraNumber == 0)
	{
		selectedCamera = camera.get();
	}
	else
	{
		std::vector<GameObject*> loadedCameras = App->scene->GetLoadedScene()->GetSceneCameras();
		if (loadedCameras.size() >= cameraNumber)
		{
			selectedCamera = (static_cast<ComponentCamera*>(loadedCameras[cameraNumber - 1]->GetComponent(ComponentType::CAMERA)))->GetCamera();
			camera->SetPosition(selectedCamera->GetPosition());
		}
	}
}

Camera* ModuleCamera::GetCamera()
{
	return selectedCamera;
}