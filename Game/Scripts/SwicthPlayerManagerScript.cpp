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
		if (input->GetKey(SDL_SCANCODE_C) != KeyState::IDLE && secondPlayer && currentPlayer->GetComponent<PlayerManagerScript>()->IsGrounded())
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

	currentPlayer->GetComponent<PlayerManagerScript>()->ParalyzePlayer(true);

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

		currentPlayer->GetComponent<PlayerManagerScript>()->ParalyzePlayer(false);
		secondPlayer->GetComponent<PlayerManagerScript>()->ParalyzePlayer(false);
	}

	else if (changePlayerTimer.Read() >= 1000 && !isNewPlayerEnabled)
	{
		jumpManager->ChangingCurrentPlayer(false);
		// The position where the newCurrentPlayer will appear
		rigidBodyVec3 = btVector3(currentPlayer->GetComponent<ComponentTransform>()->GetGlobalPosition().x,
			currentPlayer->GetComponent<ComponentTransform>()->GetGlobalPosition().y, currentPlayer->GetComponent<ComponentTransform>()->GetGlobalPosition().z);
		
		ComponentTransform* compTrans = currentPlayer->GetComponent<ComponentTransform>();
		// Disabling the current player
		currentPlayer->GetComponent<ComponentPlayer>()->SetActualPlayer(false);

		currentPlayer->Disable();
		
		// Enabling the new current player
		secondPlayer->Enable();
		secondPlayer->GetComponent<ComponentPlayer>()->SetActualPlayer(true);

		secondPlayer->GetComponent<PlayerManagerScript>()->ParalyzePlayer(true);

		secondPlayer->GetComponent<ComponentTransform>()->SetGlobalPosition(compTrans->GetGlobalPosition());
		secondPlayer->GetComponent<ComponentTransform>()->SetGlobalRotation(compTrans->GetGlobalRotation());
		secondPlayer->GetComponent<ComponentRigidBody>()->UpdateRigidBody();
		isNewPlayerEnabled = !isNewPlayerEnabled;
	}
}