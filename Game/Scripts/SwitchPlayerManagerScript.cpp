#include "StdAfx.h"
#include "SwitchPlayerManagerScript.h"

#include "ModulePlayer.h"
#include "ModuleInput.h"
#include "ModuleScene.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/UI/ComponentTransform2D.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentPlayer.h"
#include "Components/ComponentParticleSystem.h"
#include "Components/ComponentAnimation.h"

#include "../Scripts/PlayerManagerScript.h"
#include "PlayerAttackScript.h"

#include "../Scripts/CameraControllerScript.h"

#include "Application.h"
#include "Scene/Scene.h"

REGISTERCLASS(SwitchPlayerManagerScript);

SwitchPlayerManagerScript::SwitchPlayerManagerScript() : Script(), camera(nullptr), input(nullptr), isSwitchAvailable(true), changingPlayerTime{ 1000.f, 2000.f},
	playerHealthBar(nullptr), secondPlayerHealthBar(nullptr)
{
	REGISTER_FIELD(isSwitchAvailable, bool);
	REGISTER_FIELD(playerHealthBar, GameObject*);
	REGISTER_FIELD(secondPlayerHealthBar, GameObject*);
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
			&& isSwitchAvailable && !playerManager->IsParalyzed())
		{
			CheckChangeCurrentPlayer();
			VisualSwitchEffect();
		}
	}
	else 
	{
		HandleChangeCurrentPlayer();

		if(isSwitchingHealthBars)
		{
			float t = (changePlayerTimer.Read() - changingPlayerTime[0]) / 1000;
			
			// Interpolate position and scale
			float3 newCurrentPlayerPosition = float3(
				Lerp(playerHealthBar->GetComponent<ComponentTransform2D>()->GetPosition().x, secondHealthBarPosition.x, t),
				Lerp(playerHealthBar->GetComponent<ComponentTransform2D>()->GetPosition().y, secondHealthBarPosition.y, t),
				secondHealthBarPosition.z
			);

			float3 newCurrentPlayerScale = float3(
				Lerp(playerHealthBar->GetComponent<ComponentTransform2D>()->GetScale().x, secondHealthBarScale.x, t),
				Lerp(playerHealthBar->GetComponent<ComponentTransform2D>()->GetScale().y, secondHealthBarScale.y, t),
				secondHealthBarScale.z
			);

			float3 newSecondPlayerPosition = float3(
				Lerp(secondPlayerHealthBar->GetComponent<ComponentTransform2D>()->GetPosition().x, currentHealthBarPosition.x, t),
				Lerp(secondPlayerHealthBar->GetComponent<ComponentTransform2D>()->GetPosition().y, currentHealthBarPosition.y, t),
				currentHealthBarPosition.z
			);

			float3 newSecondPlayerScale = float3(
				Lerp(secondPlayerHealthBar->GetComponent<ComponentTransform2D>()->GetScale().x, currentHealthBarScale.x, t),
				Lerp(secondPlayerHealthBar->GetComponent<ComponentTransform2D>()->GetScale().y, currentHealthBarScale.y, t),
				secondHealthBarScale.z
			);

			// Set the interpolated values
			playerHealthBar->GetComponent<ComponentTransform2D>()->SetPosition(newCurrentPlayerPosition);
			playerHealthBar->GetComponent<ComponentTransform2D>()->SetScale(newCurrentPlayerScale);
			playerHealthBar->GetComponent<ComponentTransform2D>()->CalculateMatrices();
			
			secondPlayerHealthBar->GetComponent<ComponentTransform2D>()->SetPosition(newSecondPlayerPosition);
			secondPlayerHealthBar->GetComponent<ComponentTransform2D>()->SetScale(newSecondPlayerScale);
			secondPlayerHealthBar->GetComponent<ComponentTransform2D>()->CalculateMatrices();

		}
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

void SwitchPlayerManagerScript::VisualSwitchEffect()
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
	componentAnimation = currentPlayer->GetComponent<ComponentAnimation>();
	componentAnimation->SetParameter("IsJumping", true);
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
	if (changePlayerTimer.Read() >= changingPlayerTime[1])
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

		// Finish Switch HealthBars
		isSwitchingHealthBars = false;
		playerHealthBar->GetComponent<ComponentTransform2D>()->SetPosition(secondHealthBarPosition);
		playerHealthBar->GetComponent<ComponentTransform2D>()->SetScale(secondHealthBarScale);
		playerHealthBar->GetComponent<ComponentTransform2D>()->CalculateMatrices();
	
		secondPlayerHealthBar->GetComponent<ComponentTransform2D>()->SetPosition(currentHealthBarPosition);
		secondPlayerHealthBar->GetComponent<ComponentTransform2D>()->SetScale(currentHealthBarScale);
		secondPlayerHealthBar->GetComponent<ComponentTransform2D>()->CalculateMatrices();
	}

	else if (changePlayerTimer.Read() >= changingPlayerTime[0] && !isNewPlayerEnabled)
	{
		// Disabling the current player
		currentPlayer->GetComponent<ComponentPlayer>()->SetActualPlayer(false);
		playerManager->ForcingJump(false);

		componentAnimation->SetParameter("IsFalling", true);
		currentPlayer->Disable();

		// Change UI of the player here
		SwitchHealthBars();
		
		// Enabling the new current player
		secondPlayer->Enable();
		secondPlayer->GetComponent<PlayerAttackScript>()->PlayWeaponSounds();;
		secondPlayer->GetComponent<ComponentPlayer>()->SetActualPlayer(true);

		secondPlayer->GetComponent<PlayerManagerScript>()->PausePlayer(true);

		secondPlayer->GetComponent<ComponentTransform>()->SetGlobalPosition(positionPlayer);
		secondPlayer->GetComponent<ComponentTransform>()->SetGlobalRotation(currentPlayer->GetComponent<ComponentTransform>()->GetGlobalRotation());
		secondPlayer->GetComponent<ComponentRigidBody>()->UpdateRigidBody();
		isNewPlayerEnabled = !isNewPlayerEnabled;
	}
}

void SwitchPlayerManagerScript::SwitchHealthBars()
{
	currentHealthBarPosition = playerHealthBar->GetComponent<ComponentTransform2D>()->GetPosition();
	currentHealthBarScale = playerHealthBar->GetComponent<ComponentTransform2D>()->GetScale();
	secondHealthBarPosition = secondPlayerHealthBar->GetComponent<ComponentTransform2D>()->GetPosition();
	secondHealthBarScale = secondPlayerHealthBar->GetComponent<ComponentTransform2D>()->GetScale();

	isSwitchingHealthBars = true;
}

GameObject* SwitchPlayerManagerScript::GetSecondPlayer()
{
	return secondPlayer;
}