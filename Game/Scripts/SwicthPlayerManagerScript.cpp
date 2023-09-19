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

#include "../Scripts/CameraControllerScript.h"

#include "Application.h"
#include "Scene/Scene.h"

REGISTERCLASS(SwitchPlayerManagerScript);

SwitchPlayerManagerScript::SwitchPlayerManagerScript() : Script(), camera(nullptr), input(nullptr), isSwitchAvailable(true)
{
	REGISTER_FIELD(isSwitchAvailable, bool);
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
	playerManager = currentPlayer->GetComponent<PlayerManagerScript>();

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
		if (input->GetKey(SDL_SCANCODE_C) != KeyState::IDLE && secondPlayer 
			&& currentPlayer->GetComponent<PlayerManagerScript>()->IsGrounded()
			&& isSwitchAvailable)
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

void SwitchPlayerManagerScript::SetIsSwitchAvailable(bool available)
{
	isSwitchAvailable = available;
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
	actualSwitchPlayersParticles->GetComponent<ComponentTransform>()->SetGlobalPosition(currentPlayer->GetComponent<ComponentTransform>()->GetGlobalPosition());
	actualSwitchPlayersParticles->GetComponent<ComponentTransform>()->RecalculateLocalMatrix();

	actualSwitchPlayersParticles->GetComponent<ComponentParticleSystem>()->Play();
}

void SwitchPlayerManagerScript::CheckChangeCurrentPlayer()
{
	camera->ToggleCameraState();
	currentPlayer->GetComponent<PlayerManagerScript>()->PausePlayer(true);
	playerManager->ForcingJump(true);

	// The position where the newCurrentPlayer will appear
	positionPlayer = currentPlayer->GetComponent<ComponentTransform>()->GetGlobalPosition();

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

		playerManager = currentPlayer->GetComponent<PlayerManagerScript>();

		currentPlayer->GetComponent<PlayerManagerScript>()->PausePlayer(false);
		secondPlayer->GetComponent<PlayerManagerScript>()->PausePlayer(false);
	}

	else if (changePlayerTimer.Read() >= 1000 && !isNewPlayerEnabled)
	{
		// Disabling the current player
		currentPlayer->GetComponent<ComponentPlayer>()->SetActualPlayer(false);
		playerManager->ForcingJump(false);

		currentPlayer->Disable();

		// Change UI of the player here
		AXO_TODO("Change UI of the player here")
		
		// Enabling the new current player
		secondPlayer->Enable();
		secondPlayer->GetComponent<ComponentPlayer>()->SetActualPlayer(true);

		secondPlayer->GetComponent<PlayerManagerScript>()->PausePlayer(true);

		secondPlayer->GetComponent<ComponentTransform>()->SetGlobalPosition(positionPlayer);
		secondPlayer->GetComponent<ComponentTransform>()->SetGlobalRotation(currentPlayer->GetComponent<ComponentTransform>()->GetGlobalRotation());
		secondPlayer->GetComponent<ComponentRigidBody>()->UpdateRigidBody();
		isNewPlayerEnabled = !isNewPlayerEnabled;
	}
}

GameObject* SwitchPlayerManagerScript::GetSecondPlayer()
{
	return secondPlayer;
}