
#include "Application.h"

#include "ModulePlayer.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "Scene/Scene.h"

#include "Camera/Camera.h"
#include "Camera/CameraGameObject.h"
#include "Components/ComponentCamera.h"
#include "GameObject/GameObject.h"

#include "DataStructures/Quadtree.h"

ModulePlayer::ModulePlayer() {};

ModulePlayer::~ModulePlayer() {
};

bool ModulePlayer::Init()
{
	return true;
}

bool ModulePlayer::Start()
{
	//Inizialice the player

	std::vector<GameObject*> cameras = App->scene->GetLoadedScene()->GetSceneCameras();
	for (GameObject* camera : cameras)
	{
		if (camera->GetParent()->GetComponent(ComponentType::PLAYER))
		{
			SetPlayer(camera->GetParent()->GetParent()->RemoveChild(camera->GetParent()));
			cameraPlayer = (static_cast<ComponentCamera*>(camera->GetComponent(ComponentType::CAMERA)))->GetCamera();
			App->scene->GetLoadedScene()->GetSceneQuadTree()->RemoveGameObjectAndChildren(camera->GetParent());
			App->camera->SetSelectedCamera(0);
		}
	}
	return true;
}

update_status ModulePlayer::Update()
{
	return update_status::UPDATE_CONTINUE;
}

GameObject* ModulePlayer::GetPlayer()
{
	return player.get();
}

void ModulePlayer::SetPlayer(std::unique_ptr<GameObject> newPlayer)
{
	player = std::move(newPlayer);;
}

Camera* ModulePlayer::GetCameraPlayer()
{
	return cameraPlayer;
}