
#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "ModulePlayer.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

#include "Camera/Camera.h"
#include "Camera/CameraGameObject.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentMeshCollider.h"
#include "Components/ComponentPlayer.h"
#include "Components/ComponentRigidBody.h"
#include "GameObject/GameObject.h"

#include "DataStructures/Quadtree.h"

#include "Components/ComponentTransform.h"

ModulePlayer::ModulePlayer() :
	cameraPlayer(nullptr),
	player(nullptr),
	componentPlayer(nullptr),
	speed(3),
	isPlayerLoad(false),
	readyToEliminate(false){};

ModulePlayer::~ModulePlayer(){};

bool ModulePlayer::Init()
{
	return true;
}

bool ModulePlayer::Start()
{
	// Initialize the player
#ifndef ENGINE
	LoadNewPlayer();
#endif // GAMEMODE
	return true;
}

update_status ModulePlayer::PreUpdate()
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModulePlayer::Update()
{
	return update_status::UPDATE_CONTINUE;
}

GameObject* ModulePlayer::GetPlayer()
{
	return player;
}

void ModulePlayer::SetPlayer(GameObject* newPlayer)
{
	player = newPlayer;
	componentPlayer = static_cast<ComponentPlayer*>(player->GetComponent(ComponentType::PLAYER));
}

Camera* ModulePlayer::GetCameraPlayer()
{
	return cameraPlayer;
}

void ModulePlayer::LoadNewPlayer()
{
	std::vector<ComponentCamera*> cameras = App->scene->GetLoadedScene()->GetSceneCameras();
	for (ComponentCamera* camera : cameras)
	{
		GameObject* parentOfOwner = camera->GetOwner()->GetParent();
		if (parentOfOwner->GetComponent(ComponentType::PLAYER))
		{
			SetPlayer(parentOfOwner);
			cameraPlayer = camera->GetCamera();
#ifdef ENGINE
			cameraPlayer->SetAspectRatio(App->editor->GetAvailableRegion().first /
										 App->editor->GetAvailableRegion().second);
			App->scene->GetLoadedScene()->GetRootQuadtree()->RemoveGameObjectAndChildren(player);
#else
			App->scene->RemoveGameObjectAndChildren(parentOfOwner);
#endif // ENGINE
			App->camera->SetSelectedCamera(0);

			if (componentPlayer->HaveMouseActivated())
			{
				App->input->SetShowCursor(true);
			}
			else
			{
				App->input->SetShowCursor(false);
			}
			isPlayerLoad = true;
			return;
		}
	}
	isPlayerLoad = false;
	ENGINE_LOG("Player is not load");
}

void ModulePlayer::UnloadNewPlayer()
{
	App->camera->SetSelectedCamera(-1);
	player = nullptr;
	isPlayerLoad = false;
}

bool ModulePlayer::IsStatic()
{
	return componentPlayer->IsStatic();
}