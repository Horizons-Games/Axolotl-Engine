#include "StdAfx.h"
#include "SwitchPlayerManagerScript.h"

#include "ModulePlayer.h"
#include "ModuleInput.h"
#include "ModuleScene.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentPlayer.h"
#include "Components/ComponentParticleSystem.h"

#include "../Scripts/PlayerManagerScript.h"
#include "../Scripts/PlayerMoveScript.h"
#include "../Scripts/PlayerJumpScript.h"

#include "../Scripts/CameraControllerScript.h"

#include "Application.h"
#include "Scene/Scene.h"

REGISTERCLASS(SwitchPlayerManagerScript);

SwitchPlayerManagerScript::SwitchPlayerManagerScript() : Script(), camera(nullptr), input(nullptr)
{
	REGISTER_FIELD(secondPlayer, GameObject*);
	REGISTER_FIELD(switchPlayersParticlesPrefab, GameObject*);
}

void SwitchPlayerManagerScript::Start()
{
	input = App->GetModule<ModuleInput>();
	
	mainCamera = App->GetModule<ModulePlayer>()->GetCameraPlayerObject();

	camera = mainCamera->GetComponent<CameraControllerScript>();
	cameraTransform = mainCamera->GetComponent<ComponentTransform>();

	currentPlayer = App->GetModule<ModulePlayer>()->GetPlayer();
	LOG_DEBUG("Player 1: {}", currentPlayer);
	LOG_DEBUG("Player 2: {}", secondPlayer);

	camera->ChangeCurrentPlayer(currentPlayer->GetComponent<ComponentTransform>());

	if (switchPlayersParticlesPrefab)
	{
		switchPlayersParticlesPrefab->Disable();
	}
}

void SwitchPlayerManagerScript::Update(float deltaTime)
{
	if (!isChangingPlayer)
	{
		if (input->GetKey(SDL_SCANCODE_C) != KeyState::IDLE && secondPlayer)
		{
			CheckChangeCurrentPlayer();
			VisualSwicthEffect();
		}
	}
	else 
	{
		HandleChangeCurrentPlayer();
	}

	if (actualSwitchPlayersParticles && actualSwitchPlayersParticles->GetComponent<ComponentParticleSystem>()->IsFinished())
	{
		App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(actualSwitchPlayersParticles);
		actualSwitchPlayersParticles = nullptr;
	}
	if (actualSwitchPlayersParticles) 
	{
		LOG_DEBUG("Global position is: {}", actualSwitchPlayersParticles->GetComponent<ComponentTransform>()->GetGlobalPosition().x);
		LOG_DEBUG("Global position is: {}", actualSwitchPlayersParticles->GetComponent<ComponentTransform>()->GetGlobalPosition().y);
		LOG_DEBUG("Global position is: {}", actualSwitchPlayersParticles->GetComponent<ComponentTransform>()->GetGlobalPosition().z);
	}
}

void SwitchPlayerManagerScript::VisualSwicthEffect()
{
	Scene* loadScene = App->GetModule<ModuleScene>()->GetLoadedScene();
	if (actualSwitchPlayersParticles)
	{
		App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(actualSwitchPlayersParticles);
	}

	actualSwitchPlayersParticles = loadScene->DuplicateGameObject(switchPlayersParticlesPrefab->GetName(), switchPlayersParticlesPrefab, loadScene->GetRoot());
	actualSwitchPlayersParticles->GetComponent<ComponentParticleSystem>()->Enable();
	actualSwitchPlayersParticles->GetComponent<ComponentRigidBody>()->SetRigidBodyOrigin(btVector3 (currentPlayer->GetComponent<ComponentTransform>()->GetGlobalPosition().x, 
		currentPlayer->GetComponent<ComponentTransform>()->GetGlobalPosition().y, currentPlayer->GetComponent<ComponentTransform>()->GetGlobalPosition().z));

	actualSwitchPlayersParticles->GetComponent<ComponentParticleSystem>()->Play();
}

void SwitchPlayerManagerScript::CheckChangeCurrentPlayer()
{
	jumpManager = currentPlayer->GetComponent<PlayerJumpScript>();
	camera->ToggleCameraState();
	jumpManager->ChangingCurrentPlayer(true);

	changePlayerTimer.Start();
	isChangingPlayer = true;
}

void SwitchPlayerManagerScript::HandleChangeCurrentPlayer()
{
	if (changePlayerTimer.Read() >= 2000)
	{	
		camera->ChangeCurrentPlayer(secondPlayer->GetComponent<ComponentTransform>());

		changePlayerTimer.Stop();
		isChangingPlayer = false;
		isNewPlayerEnabled = !isNewPlayerEnabled;
		GameObject* changePlayerGameObject = currentPlayer;
		currentPlayer = App->GetModule<ModulePlayer>()->GetPlayer();
		secondPlayer = changePlayerGameObject;
	}

	else if (changePlayerTimer.Read() >= 1500 && !isNewPlayerEnabled)
	{
		// The position where the newCurrentPlayer will appear
		rigidBodyVec3 = btVector3(currentPlayer->GetComponent<ComponentTransform>()->GetGlobalPosition().x,
			currentPlayer->GetComponent<ComponentTransform>()->GetGlobalPosition().y, currentPlayer->GetComponent<ComponentTransform>()->GetGlobalPosition().z);

		// Disabling the current player
		currentPlayer->GetComponent<ComponentPlayer>()->SetActualPlayer(false);

		currentPlayer->Disable();
		
		// Enabling the new current player
		secondPlayer->Enable();
		secondPlayer->GetComponent<ComponentPlayer>()->SetActualPlayer(true);

		secondPlayer->GetComponent<ComponentRigidBody>()->SetRigidBodyOrigin(rigidBodyVec3);
		isNewPlayerEnabled = !isNewPlayerEnabled;
	}

	else if (changePlayerTimer.Read() >= 1000 && !isNewPlayerEnabled)
	{
		jumpManager->ChangingCurrentPlayer(false);
	}
}