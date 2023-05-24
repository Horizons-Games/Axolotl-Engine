
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

ModulePlayer::ModulePlayer() : cameraPlayer(nullptr), player(nullptr), componentPlayer(nullptr), speed(3){};

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

void ModulePlayer::LoadNewPlayer()
{
	ModuleScene* scene = App->GetModule<ModuleScene>();
	Scene* loadedScene = scene->GetLoadedScene();
	ModuleInput* input = App->GetModule<ModuleInput>();
	ModuleEditor* editor = App->GetModule<ModuleEditor>();
	std::vector<ComponentCamera*> cameras = loadedScene->GetSceneCameras();
	for (ComponentCamera* camera : cameras)
	{
		GameObject* parentOfOwner = camera->GetOwner()->GetParent();
		if (parentOfOwner->GetComponent<ComponentPlayer>())
		{
			SetPlayer(parentOfOwner);
			cameraPlayer = camera->GetCamera();
#ifdef ENGINE
			cameraPlayer->SetAspectRatio(editor->GetAvailableRegion().first / editor->GetAvailableRegion().second);
			loadedScene->GetRootQuadtree()->RemoveGameObjectAndChildren(player);
#else
			scene->RemoveGameObjectAndChildren(player);
#endif // ENGINE			
			App->GetModule<ModuleCamera>()->SetSelectedCamera(0);

			if (componentPlayer->HaveMouseActivated())
			{
				input->SetShowCursor(true);
			}
			else
			{
				input->SetShowCursor(false);
			}
			return;
		}
	}
	ENGINE_LOG("Player is not load");
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