#include "StdAfx.h"

#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "ModulePlayer.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

#include "Camera/Camera.h"
#include "Camera/CameraGameObject.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentPlayer.h"
#include "GameObject/GameObject.h"

#include "DataStructures/Quadtree.h"

#include "Components/ComponentTransform.h"

ModulePlayer::ModulePlayer() : cameraPlayer(nullptr), player(nullptr), componentPlayer(nullptr)
{
};

ModulePlayer::~ModulePlayer(){};

bool ModulePlayer::Start()
{
	// Initialize the player
#ifndef ENGINE
	LoadNewPlayer();
#endif // GAMEMODE
	return true;
}

GameObject* ModulePlayer::GetPlayer()
{
	return player;
}

void ModulePlayer::SetPlayer(GameObject* newPlayer)
{
	player = newPlayer;
	componentPlayer = player->GetComponent<ComponentPlayer>();
}

Camera* ModulePlayer::GetCameraPlayer()
{
	return cameraPlayer;
}

GameObject* ModulePlayer::GetCameraPlayerObject()
{
	return cameraPlayerObject;
}

void ModulePlayer::LoadNewPlayer()
{
	ModuleScene* scene = App->GetModule<ModuleScene>();
	Scene* loadedScene = scene->GetLoadedScene();
	ModuleEditor* editor = App->GetModule<ModuleEditor>();
	std::vector<ComponentCamera*> cameras = loadedScene->GetSceneCameras();

	GameObject* player = loadedScene->GetPlayer();
	if (player)
	{
		SetPlayer(player);

		for (ComponentCamera* camera : cameras)
		{
			GameObject* ownerGO = camera->GetOwner();
			if (ownerGO->CompareTag("MainCamera"))
			{
				cameraPlayer = camera->GetCamera();
				cameraPlayerObject = ownerGO;
#ifdef ENGINE
				cameraPlayer->SetAspectRatio(editor->GetAvailableRegion().first / editor->GetAvailableRegion().second);
				// loadedScene->GetRootQuadtree()->RemoveGameObjectAndChildren(player);
#else
				// scene->RemoveGameObjectAndChildren(player);
#endif // ENGINE
				App->GetModule<ModuleCamera>()->SetSelectedCamera(0);

				CheckIfActivateMouse();

				return;
			}
		}
	}
	LOG_ERROR("Player is not loaded");
}

void ModulePlayer::UnloadNewPlayer()
{
	App->GetModule<ModuleCamera>()->SetSelectedCamera(-1);
	player = nullptr;
}

bool ModulePlayer::IsStatic()
{
	return componentPlayer->IsStatic();
}

void ModulePlayer::CheckIfActivateMouse()
{
	if (componentPlayer->HaveMouseActivated())
	{
		App->GetModule<ModuleInput>()->SetShowCursor(true);
	}
	else
	{
		App->GetModule<ModuleInput>()->SetShowCursor(false);
	}

}
