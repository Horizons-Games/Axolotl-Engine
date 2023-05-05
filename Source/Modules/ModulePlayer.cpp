
#include "Application.h"

#include "ModulePlayer.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleRender.h"
#include "Scene/Scene.h"
#include "ModuleInput.h"

#include "Camera/Camera.h"
#include "Camera/CameraGameObject.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentPlayer.h"
#include "Components/ComponentMeshCollider.h"
#include "Components/ComponentRigidBody.h"
#include "GameObject/GameObject.h"

#include "DataStructures/Quadtree.h"

#include "Components/ComponentTransform.h"

ModulePlayer::ModulePlayer(): cameraPlayer(nullptr), player(nullptr),
	componentPlayer(nullptr), speed(3), isPlayerLoad(false), readyToEliminate(false){};

ModulePlayer::~ModulePlayer() {
};

bool ModulePlayer::Start()
{
	//Initialize the player
#ifndef ENGINE
	LoadNewPlayer();
#endif //GAMEMODE
	return true;
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

void ModulePlayer::LoadNewPlayer()
{
	ModuleScene* scene = App->GetModule<ModuleScene>();
	ModuleInput* input = App->GetModule<ModuleInput>();
	ModuleEditor* editor = App->GetModule<ModuleEditor>();
	std::vector<ComponentCamera*> cameras = scene->GetLoadedScene()->GetSceneCameras();
	for (ComponentCamera* camera : cameras)
	{
		GameObject* parentOfOwner = camera->GetOwner()->GetParent();
		if (parentOfOwner->GetComponent(ComponentType::PLAYER))
		{
			SetPlayer(parentOfOwner);
			cameraPlayer = camera->GetCamera();
#ifdef ENGINE
			cameraPlayer->SetAspectRatio(editor->GetAvailableRegion().first / editor->GetAvailableRegion().second);
			scene->GetLoadedScene()->GetRootQuadtree()->RemoveGameObjectAndChildren(player);
#else
			scene->RemoveGameObjectAndChildren(parentOfOwner);
#endif // ENGINE			
			App->GetModule<ModuleCamera>()->SetSelectedCamera(0);
			
			if(componentPlayer->HaveMouseActivated()) 
			{
				input->SetShowCursor(true);
			}
			else 
			{
				input->SetShowCursor(false);
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
	App->GetModule<ModuleCamera>()->SetSelectedCamera(-1);
	player = nullptr;
	isPlayerLoad = false;
}

bool ModulePlayer::IsStatic()
{
	return componentPlayer->IsStatic();
}