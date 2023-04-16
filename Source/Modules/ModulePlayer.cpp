
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
#include "GameObject/GameObject.h"

#include "DataStructures/Quadtree.h"

#include "Components/ComponentTransform.h"

ModulePlayer::ModulePlayer(): cameraPlayer(nullptr), player(nullptr), lastPlayer(nullptr),
	componentPlayer(nullptr), speed(3), isPlayerLoad(false), readyToEliminate(false){};

ModulePlayer::~ModulePlayer() {
};

bool ModulePlayer::Init()
{
	return true;
}

bool ModulePlayer::Start()
{
	//Initialize the player
	LoadNewPlayer();
	return true;
}

update_status ModulePlayer::PreUpdate()
{
#ifdef ENGINE
	if (isPlayerLoad && App->GetIsOnPlayMode())
	{
		if (player && !componentPlayer->IsStatic() && App->camera->GetSelectedPosition() == 0)
		{
			Move();
			Rotate();
		}
	}

#else //ENGINE
	if (player && !componentPlayer->IsStatic() && App->camera->GetSelectedPosition() == 0)
	{
		Move();
		Rotate();
	}
#endif //GAMEMODE
	return update_status::UPDATE_CONTINUE;
}

update_status ModulePlayer::Update()
{
#ifdef ENGINE
	if (isPlayerLoad && App->GetIsOnPlayMode())
	{
		ComponentTransform* trans = static_cast<ComponentTransform*>(player->GetComponent(ComponentType::TRANSFORM));
		trans->UpdateTransformMatrices();
		
		if (readyToEliminate)
		{
			App->OnStopPlay();
			readyToEliminate = false;
		}
	}
	
#else //ENGINE
	if (player)
	{
		player->Update();
		ComponentTransform* trans = static_cast<ComponentTransform*>(player->GetComponent(ComponentType::TRANSFORM));
		trans->UpdateTransformMatrices();
	}
#endif //GAMEMODE
	return update_status::UPDATE_CONTINUE;
}

GameObject* ModulePlayer::GetPlayer()
{
	return player.get();
}

void ModulePlayer::SetPlayer(std::unique_ptr<GameObject> newPlayer)
{
	player = std::move(newPlayer);
	componentPlayer = static_cast<ComponentPlayer*>(player->GetComponent(ComponentType::PLAYER));
}

Camera* ModulePlayer::GetCameraPlayer()
{
	return cameraPlayer;
}

void ModulePlayer::Move()
{
	float deltaTime = (App->GetDeltaTime() < 1.f)?App->GetDeltaTime():1.f;
	ComponentTransform* trans = static_cast<ComponentTransform*>(player->GetComponent(ComponentType::TRANSFORM));
	float3 position = trans->GetPosition();
	//Forward
	if (App->input->GetKey(SDL_SCANCODE_W) != KeyState::IDLE)
	{
		position += trans->GetGlobalForward().Normalized() * speed * deltaTime;
		trans->SetPosition(position);
		//trans->UpdateTransformMatrices();
	}

	//Backward
	if (App->input->GetKey(SDL_SCANCODE_S) != KeyState::IDLE)
	{
		position += -trans->GetGlobalForward().Normalized() * speed * deltaTime;
		trans->SetPosition(position);
		//trans->UpdateTransformMatrices();
	}

	//Left
	if (App->input->GetKey(SDL_SCANCODE_A) != KeyState::IDLE)
	{
		position += trans->GetGlobalRight().Normalized() * speed*2/3 * deltaTime;
		trans->SetPosition(position);
		//trans->UpdateTransformMatrices();
	}

	//Right
	if (App->input->GetKey(SDL_SCANCODE_D) != KeyState::IDLE)
	{
		position += -trans->GetGlobalRight().Normalized() * speed*2/3 * deltaTime;
		trans->SetPosition(position);
		//trans->UpdateTransformMatrices();
	}
}

void ModulePlayer::Rotate()
{
	if (App->input->GetMouseMotion().x != 0)
	{
		float deltaTime = App->GetDeltaTime();
		ComponentTransform* trans = static_cast<ComponentTransform*>(player->GetComponent(ComponentType::TRANSFORM));
		float3 newRot = trans->GetRotationXYZ();
		newRot.y += - App->input->GetMouseMotion().x * deltaTime;
		trans->SetRotation(newRot);
		//trans->UpdateTransformMatrices();
	}
}

void ModulePlayer::LoadNewPlayer()
{
	std::vector<GameObject*> cameras = App->scene->GetLoadedScene()->GetSceneCameras();
	for (GameObject* camera : cameras)
	{
		if (camera->GetParent()->GetComponent(ComponentType::PLAYER))
		{
#ifdef ENGINE
			SetPlayer(std::make_unique<GameObject>(static_cast<GameObject&>(*camera->GetParent())));
			lastPlayer = camera->GetParent();
			// look for the player's camera
			std::vector<GameObject*> children = player.get()->GetChildren();
			for (auto child : children)
			{
				if (!child->GetComponentsByType<ComponentCamera>(ComponentType::CAMERA).empty())
				{
					cameraPlayer = static_cast<ComponentCamera*>(child->GetComponent(ComponentType::CAMERA))->GetCamera();
					cameraPlayer->SetAspectRatio(App->editor->GetAvailableRegion().first / App->editor->GetAvailableRegion().second);
				}
			}

			lastPlayer->Disable();
			App->camera->SetSelectedCamera(0);
#else
			SetPlayer(camera->GetParent()->GetParent()->RemoveChild(camera->GetParent()));
			cameraPlayer = static_cast<ComponentCamera*>(camera->GetComponent(ComponentType::CAMERA))->GetCamera();
			App->scene->GetLoadedScene()->GetRootQuadtree()->RemoveGameObjectAndChildren(camera->GetParent());
			App->camera->SetSelectedCamera(0);

#endif // ENGINE			
			if(componentPlayer->HaveMouseActivated()) 
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
	lastPlayer->Enable();
	player = nullptr;
	isPlayerLoad = false;
}

bool ModulePlayer::IsStatic()
{
	return componentPlayer->IsStatic();
}