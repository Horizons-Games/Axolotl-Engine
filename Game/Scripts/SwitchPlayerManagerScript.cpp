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
#include "ParticleBillboardAssistance.h"

#include "../Scripts/CameraControllerScript.h"
#include "../Scripts/CameraBossControllerScript.h"
#include "ComboManager.h"

#include "Application.h"
#include "Scene/Scene.h"

REGISTERCLASS(SwitchPlayerManagerScript);

SwitchPlayerManagerScript::SwitchPlayerManagerScript() : Script(), camera(nullptr), input(nullptr),
	modulePlayer(nullptr), isSwitchAvailable(true), changingPlayerTime{200.f, 800.f, 1800.f},
	currentPlayerHealthBar(nullptr), secondPlayerHealthBar(nullptr), currentHealthBarTransform(nullptr),
	secondHealthBarTransform(nullptr), currentPlayerTransform(nullptr), secondPlayerTransform(nullptr),
	particlesTransform(nullptr), isSecondJumpAvailable(true), comboSystem(nullptr),
	cameraBoss(nullptr), bossScene(false), currentChangePlayerTime(0)
{
	REGISTER_FIELD(isSwitchAvailable, bool);
	REGISTER_FIELD(bossScene, bool);
	REGISTER_FIELD(currentPlayerHealthBar, GameObject*);
	REGISTER_FIELD(secondPlayerHealthBar, GameObject*);
	REGISTER_FIELD(secondPlayer, GameObject*);
	REGISTER_FIELD(switchPlayersParticlesPrefab, GameObject*);
}

void SwitchPlayerManagerScript::Start()
{
	input = App->GetModule<ModuleInput>();
	modulePlayer = App->GetModule<ModulePlayer>();

	currentPlayer = modulePlayer->GetPlayer();

	comboSystem = currentPlayer->GetComponent<ComboManager>();

	currentHealthBarTransform = currentPlayerHealthBar->GetComponent<ComponentTransform2D>();
	secondHealthBarTransform = secondPlayerHealthBar->GetComponent<ComponentTransform2D>();
	currentPlayerTransform = currentPlayer->GetComponent<ComponentTransform>();
	secondPlayerTransform = secondPlayer->GetComponent<ComponentTransform>();
	
	mainCamera = modulePlayer->GetCameraPlayerObject();


	if (bossScene)
	{
		cameraBoss = mainCamera->GetComponent<CameraBossControllerScript>();
	}
	else
	{
		camera = mainCamera->GetComponent<CameraControllerScript>();
	}

	playerManager = currentPlayer->GetComponent<PlayerManagerScript>();


	if (switchPlayersParticlesPrefab)
	{
		switchPlayersParticlesPrefab->Disable();
	}

	if (!isSwitchAvailable)
	{
		secondPlayerHealthBar->Disable();
	}
}

void SwitchPlayerManagerScript::Update(float deltaTime)
{
	if (playerManager->IsPaused())
	{
		if (!isPaused)
		{
			changePlayerTimer.Pause();
		}
		isPaused = true;
		return;
	}
	if (isPaused)
	{
		changePlayerTimer.Play();
	}
	
	isPaused = false;
	if (!isChangingPlayer)
	{
		if (input->GetKey(SDL_SCANCODE_C) != KeyState::IDLE && secondPlayer 
			&& playerManager->IsGrounded()
			&& playerManager->GetPlayerState() != PlayerActions::DASHING
			&& playerManager->GetAttackManager()->IsAttackAvailable()
			&& isSwitchAvailable)
		{
			CheckChangeCurrentPlayer();
		}
	}
	else 
	{
		HandleChangeCurrentPlayer();

		if (isSwitchingHealthBars)
		{
			float switchingBarsTime = (changePlayerTimer.Read() - changingPlayerTime[0]) / 1000;
			
			// Interpolate position and scale
			float3 newCurrentPlayerPosition = float3(
				Lerp(currentHealthBarTransform->GetPosition().x, secondHealthBarPosition.x, switchingBarsTime),
				Lerp(currentHealthBarTransform->GetPosition().y, secondHealthBarPosition.y, switchingBarsTime),
				secondHealthBarPosition.z
			);

			float3 newCurrentPlayerScale = float3(
				Lerp(currentHealthBarTransform->GetScale().x, secondHealthBarScale.x, switchingBarsTime),
				Lerp(currentHealthBarTransform->GetScale().y, secondHealthBarScale.y, switchingBarsTime),
				secondHealthBarScale.z
			);

			float3 newSecondPlayerPosition = float3(
				Lerp(secondHealthBarTransform->GetPosition().x, currentHealthBarPosition.x, switchingBarsTime),
				Lerp(secondHealthBarTransform->GetPosition().y, currentHealthBarPosition.y, switchingBarsTime),
				currentHealthBarPosition.z
			);

			float3 newSecondPlayerScale = float3(
				Lerp(secondHealthBarTransform->GetScale().x, currentHealthBarScale.x, switchingBarsTime),
				Lerp(secondHealthBarTransform->GetScale().y, currentHealthBarScale.y, switchingBarsTime),
				secondHealthBarScale.z
			);

			// Set the interpolated values
			currentHealthBarTransform->SetPosition(newCurrentPlayerPosition);
			currentHealthBarTransform->SetScale(newCurrentPlayerScale);
			currentHealthBarTransform->CalculateMatrices();
			
			secondHealthBarTransform->SetPosition(newSecondPlayerPosition);
			secondHealthBarTransform->SetScale(newSecondPlayerScale);
			secondHealthBarTransform->CalculateMatrices();

		}
		currentChangePlayerTime = changePlayerTimer.Read();
	}
	if (actualSwitchPlayersParticles && 
		actualSwitchPlayersParticles->GetChildren()[0]->GetComponent<ComponentParticleSystem>()->IsFinished())
	{
		App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(actualSwitchPlayersParticles);
		actualSwitchPlayersParticles = nullptr;
	}

}

void SwitchPlayerManagerScript::SetIsSwitchAvailable(bool available)
{
	isSwitchAvailable = available;
	secondPlayerHealthBar->Enable();
}

void SwitchPlayerManagerScript::VisualSwitchEffect()
{
	Scene* loadScene = App->GetModule<ModuleScene>()->GetLoadedScene();
	if (actualSwitchPlayersParticles)
	{
		loadScene->DestroyGameObject(actualSwitchPlayersParticles);
	}

	actualSwitchPlayersParticles = loadScene->DuplicateGameObject(switchPlayersParticlesPrefab->GetName(),
		switchPlayersParticlesPrefab, loadScene->GetRoot());
	particlesTransform = actualSwitchPlayersParticles->GetComponent<ComponentTransform>();

	particlesTransform->SetGlobalPosition(float3 (currentPlayerTransform->GetGlobalPosition().x,
		currentPlayerTransform->GetGlobalPosition().y + 0.9f, currentPlayerTransform->GetGlobalPosition().z));
	particlesTransform->RecalculateLocalMatrix();
	particlesTransform->UpdateTransformMatrices();
	actualSwitchPlayersParticles->GetComponent<ParticleBillboardAssistance>()->UpdateTransform();
	actualSwitchPlayersParticles->GetChildren()[0]->GetComponent<ComponentParticleSystem>()->Enable();
	actualSwitchPlayersParticles->GetChildren()[0]->GetComponent<ComponentParticleSystem>()->Play();
}

void SwitchPlayerManagerScript::CheckChangeCurrentPlayer()
{
	componentAnimation = currentPlayer->GetComponent<ComponentAnimation>();
	componentAnimation->SetParameter("IsJumping", true);
	if (camera)
	{
		camera->ToggleCameraState();
	}
	else
	{
		cameraBoss->ToggleCameraState();
	}
	currentPlayer->GetComponent<PlayerManagerScript>()->PausePlayer(true);
	playerManager->TriggerJump(true);

	currentPlayerTransform = currentPlayer->GetComponent<ComponentTransform>();
	secondPlayerTransform = secondPlayer->GetComponent<ComponentTransform>();

	// The position where the newCurrentPlayer will appear
	playerPosition = currentPlayerTransform->GetGlobalPosition();

	changePlayerTimer.Stop();
	changePlayerTimer.Start();
	isChangingPlayer = true;
}

void SwitchPlayerManagerScript::HandleChangeCurrentPlayer()
{
	if (changePlayerTimer.Read() >= changingPlayerTime[2])
	{	
		if (camera)
		{
			camera->ChangeCurrentPlayer(secondPlayerTransform);
		}
		else
		{
			cameraBoss->ChangeCurrentPlayer(secondPlayerTransform);
		}
		
		changePlayerTimer.Stop();
		isChangingPlayer = false;
		isNewPlayerEnabled = !isNewPlayerEnabled;
		isSecondJumpAvailable = !isSecondJumpAvailable;
		GameObject* changePlayerGameObject = currentPlayer;
		currentPlayer = modulePlayer->GetPlayer();
		secondPlayer = changePlayerGameObject;

		playerManager = currentPlayer->GetComponent<PlayerManagerScript>();

		currentPlayer->GetComponent<PlayerManagerScript>()->PausePlayer(false);
		secondPlayer->GetComponent<PlayerManagerScript>()->PausePlayer(false);

		comboSystem = currentPlayer->GetComponent<ComboManager>();

		currentChangePlayerTime = 0;

		// Finish Switch HealthBars
		isSwitchingHealthBars = false;
		currentHealthBarTransform->SetPosition(secondHealthBarPosition);
		currentHealthBarTransform->SetScale(secondHealthBarScale);
		currentHealthBarTransform->CalculateMatrices();
	
		secondHealthBarTransform->SetPosition(currentHealthBarPosition);
		secondHealthBarTransform->SetScale(currentHealthBarScale);
		secondHealthBarTransform->CalculateMatrices();
	}
	else if (changePlayerTimer.Read() >= changingPlayerTime[1] && !isNewPlayerEnabled)
	{
		// Disabling the current player
		currentPlayer->GetComponent<ComponentPlayer>()->SetActualPlayer(false);
		playerManager->TriggerJump(false);

		componentAnimation->SetParameter("IsFalling", true);
		VisualSwitchEffect();
		comboSystem->ClearComboForSwitch(true);

		currentPlayer->Disable();
		playerManager->StopHackingParticle();

		// Change UI of the player here
		SwitchHealthBars();
		
		// Enabling the new current player
		secondPlayer->GetComponent<ComponentPlayer>()->SetActualPlayer(true);
		secondPlayer->Enable();
		secondPlayer->GetComponent<PlayerAttackScript>()->PlayWeaponSounds();;

		secondPlayer->GetComponent<PlayerManagerScript>()->PausePlayer(true);

		playerPosition.y += 0.5f;
		secondPlayerTransform->SetGlobalPosition(playerPosition);
		secondPlayerTransform->SetGlobalRotation(currentPlayerTransform->GetGlobalRotation());
		secondPlayer->GetComponent<ComponentRigidBody>()->UpdateRigidBody();
		isNewPlayerEnabled = !isNewPlayerEnabled;
	}
	else if (changePlayerTimer.Read() >= changingPlayerTime[0] && isSecondJumpAvailable)
	{
		playerManager->TriggerJump(true);
		isSecondJumpAvailable = !isSecondJumpAvailable;
	}
}

void SwitchPlayerManagerScript::SwitchHealthBars()
{
	currentHealthBarPosition = currentHealthBarTransform->GetPosition();
	currentHealthBarScale = currentHealthBarTransform->GetScale();
	secondHealthBarPosition = secondHealthBarTransform->GetPosition();
	secondHealthBarScale = secondHealthBarTransform->GetScale();

	isSwitchingHealthBars = true;
}

GameObject* SwitchPlayerManagerScript::GetSecondPlayer() const
{
	return secondPlayer;
}