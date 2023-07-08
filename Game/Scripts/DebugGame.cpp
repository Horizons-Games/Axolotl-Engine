#include "StdAfx.h"

#include "DebugGame.h"
#include "Components/ComponentPlayer.h"
#include "ModuleInput.h"
#include "HealthSystem.h"
#include "ModuleCamera.h"
#include "BixAttackScript.h"
#include "PlayerMoveScript.h"
#include "PlayerJumpScript.h"
#include "PlayerRotationScript.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "debugdraw.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "../Scripts/PowerUpLogicScript.h"

#include "GameManager.h"

REGISTERCLASS(DebugGame);


DebugGame::DebugGame() : Script(), isDebugModeActive(false), debugCurrentPosIndex(0), playerOnLocation(true), DebugPowerUp(nullptr)
{
	
	REGISTER_FIELD(debugPoint1, GameObject*);
	REGISTER_FIELD(debugPoint2, GameObject*);
	REGISTER_FIELD(debugPoint3, GameObject*);
	REGISTER_FIELD(debugPoint4, GameObject*);
	REGISTER_FIELD(debugPoint5, GameObject*);
	REGISTER_FIELD(isDebugModeActive, bool);
	REGISTER_FIELD(playerOnLocation, bool);
	REGISTER_FIELD(DebugPowerUp, GameObject*)
}

void DebugGame::Start()
{
	input = App->GetModule<ModuleInput>();

	//ImmortalityStart
	player = App->GetModule<ModulePlayer>()->GetPlayer();

	playerHealthSystem = player->GetComponent<HealthSystem>();
	playerAttackScript = player->GetComponent<BixAttackScript>();
	playerMoveScript = player->GetComponent<PlayerMoveScript>();
	playerJumpScript = player->GetComponent<PlayerJumpScript>();
	playerRotationScript = player->GetComponent<PlayerRotationScript>();

	//TeleportStart

	if (debugPoint1)
	{
		debugPoints.push_back(debugPoint1->GetComponent<ComponentTransform>());
	}

	if (debugPoint2)
	{
		debugPoints.push_back(debugPoint2->GetComponent<ComponentTransform>());
	}

	if (debugPoint3)
	{
		debugPoints.push_back(debugPoint3->GetComponent<ComponentTransform>());
	}

	if (debugPoint4)
	{
		debugPoints.push_back(debugPoint4->GetComponent<ComponentTransform>());
	}

	if (debugPoint5)
	{
		debugPoints.push_back(debugPoint5->GetComponent<ComponentTransform>());
	}

	playerRigidBody = player->GetComponent<ComponentRigidBody>();
	playerTransform = player->GetComponent<ComponentTransform>();

	currentdDebugPointTransform = debugPoints.front();	
}

void DebugGame::Update(float deltaTime)
{
	
	
	//INPUTS

	if (input->GetKey(SDL_SCANCODE_B) == KeyState::DOWN)
	{
		SwitchDebugMode();
	}

	if (input->GetKey(SDL_SCANCODE_4) == KeyState::DOWN && isDebugModeActive)
	{
		GodCamera();
	}

	if (input->GetKey(SDL_SCANCODE_5) == KeyState::DOWN && isDebugModeActive)
	{
		PowerUpDrop();
	}

	if (input->GetKey(SDL_SCANCODE_7) == KeyState::DOWN && isDebugModeActive)
	{
		FillHealth();
	}

	if (input->GetKey(SDL_SCANCODE_8) == KeyState::DOWN && isDebugModeActive)
	{

		BeImmortal();
	}

	if (input->GetKey(SDL_SCANCODE_9) == KeyState::DOWN && isDebugModeActive)
	{
		
		DeathTouch();
	}

	if (input->GetKey(SDL_SCANCODE_0) == KeyState::DOWN && isDebugModeActive)
	{
		Teleport();
		playerOnLocation = false;
		
	}

	//TELEPORTMOVEMENT
	if(!playerOnLocation && isDebugModeActive)
	{
		for(const ComponentTransform* debugPointTransform : debugPoints)
		{
			if (playerTransform->GetGlobalPosition().Equals(debugPointTransform->GetGlobalPosition(), 2.0f))
			{
				playerRigidBody->SetIsTrigger(false);
				playerRigidBody->DisablePositionController();
				playerOnLocation = true;
				break;
			}
		}
	}
	


}

void DebugGame::SwitchDebugMode()
{
	if (!isDebugModeActive)
	{
		isDebugModeActive = true;
		LOG_DEBUG("DEBUG MODE ACTIVATED");
	}
	else
	{
		isDebugModeActive = false;
		LOG_DEBUG("DEBUG MODE DEACTIVATED");
	}
}

void DebugGame::GodCamera() const {

	ModuleCamera* camera = App->GetModule<ModuleCamera>();


	if (!playerMoveScript->GetIsParalized() && playerJumpScript->GetCanJump())
	{
		playerMoveScript->SetIsParalized(true);
		playerJumpScript->SetCanJump(false);
		
		camera->SetSelectedPosition(1);
		camera->SetSelectedCamera(camera->GetSelectedPosition());
		LOG_DEBUG("GOD CAMERA ACTIVATED");
	}
	else if (playerMoveScript->GetIsParalized() && !playerJumpScript->GetCanJump())
	{
		playerMoveScript->SetIsParalized(false);
		playerJumpScript->SetCanJump(true);
		
		camera->SetSelectedPosition(0);
		camera->SetSelectedCamera(camera->GetSelectedPosition());
		LOG_DEBUG("GOD CAMERA DEACTIVATED");
	}

	

}

void DebugGame::PowerUpDrop() const
{

	if (DebugPowerUp != nullptr)
	{
		PowerUpLogicScript* newPowerUpLogic = DebugPowerUp->GetComponent<PowerUpLogicScript>();
		ComponentTransform* ownerTransform = player->GetComponent<ComponentTransform>();

		newPowerUpLogic->ActivatePowerUp(ownerTransform->GetPosition());
	}


}

void DebugGame::FillHealth() const
{
	playerHealthSystem->HealLife(playerHealthSystem->GetMaxHealth());
	LOG_DEBUG("Full Health");
}

void DebugGame::BeImmortal() const
{
	if (!playerHealthSystem->GetIsImmortal()) 
	{
		playerHealthSystem->SetIsImmortal(true);
		LOG_DEBUG("Immortal ON");
	}
	else if (playerHealthSystem->GetIsImmortal())
	{
		playerHealthSystem->SetIsImmortal(false);
		LOG_DEBUG("Immortal OFF");
	}
}

void DebugGame::DeathTouch() const
{

	if (!playerAttackScript->GetIsDeathTouched())
	{
		playerAttackScript->SetIsDeathTouched(true);
		LOG_DEBUG("Death Touch ON");
	}
	else if (playerAttackScript->GetIsDeathTouched())
	{
		playerAttackScript->SetIsDeathTouched(false);
		LOG_DEBUG("Death Touch OFF");	
	}

}

void DebugGame::Teleport() 
{
	LOG_DEBUG("TELEPORTING");
	playerRigidBody->SetIsTrigger(true);
	currentdDebugPointTransform = debugPoints[debugCurrentPosIndex];
	playerRigidBody->SetPositionTarget(currentdDebugPointTransform->GetGlobalPosition());
	debugCurrentPosIndex = (debugCurrentPosIndex + 1) % debugPoints.size();
	LOG_DEBUG("%d", debugCurrentPosIndex);
}





