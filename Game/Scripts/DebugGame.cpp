#include "DebugGame.h"

#include "Application.h"
#include "ModulePlayer.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"

#include "Components/ComponentPlayer.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

#include "../Scripts/HealthSystem.h"
#include "../Scripts/PlayerAttackScript.h"
#include "../Scripts/PlayerMoveScript.h"
#include "../Scripts/PlayerJumpScript.h"
#include "../Scripts/PlayerRotationScript.h"
#include "../Scripts/PowerUpLogicScript.h"
#include "../Scripts/GameManager.h"

#include "debugdraw.h"
#include "AxoLog.h"

REGISTERCLASS(DebugGame);

DebugGame::DebugGame() : Script(), isDebugModeActive(false), debugCurrentPosIndex(0), playerOnLocation(true), 
	debugPowerUp(nullptr)
{
	REGISTER_FIELD(debugPoints, std::vector<ComponentTransform*>);

	REGISTER_FIELD(isDebugModeActive, bool);
	REGISTER_FIELD(playerOnLocation, bool);
	REGISTER_FIELD(debugPowerUp, GameObject*)
}

void DebugGame::Start()
{
	input = App->GetModule<ModuleInput>();
	player = App->GetModule<ModulePlayer>()->GetPlayer();

	playerHealthSystem = player->GetComponent<HealthSystem>();
	playerAttackScript = player->GetComponent<PlayerAttackScript>();
	playerMoveScript = player->GetComponent<PlayerMoveScript>();
	playerJumpScript = player->GetComponent<PlayerJumpScript>();
	playerRotationScript = player->GetComponent<PlayerRotationScript>();

	playerRigidBody = player->GetComponent<ComponentRigidBody>();
	playerTransform = player->GetComponent<ComponentTransform>();

	currentdDebugPointTransform = debugPoints.front();	
}

void DebugGame::Update(float deltaTime)
{
	if (input->GetKey(SDL_SCANCODE_B) == KeyState::DOWN)
	{
		SwitchDebugMode();
	}

	if (!isDebugModeActive)
	{
		return;
	}

	if (input->GetKey(SDL_SCANCODE_4) == KeyState::DOWN)
	{
		GodCamera();
	}

	if (input->GetKey(SDL_SCANCODE_5) == KeyState::DOWN)
	{
		PowerUpDrop();
	}

	if (input->GetKey(SDL_SCANCODE_7) == KeyState::DOWN)
	{
		FillHealth();
	}

	if (input->GetKey(SDL_SCANCODE_8) == KeyState::DOWN)
	{
		BeImmortal();
	}

	if (input->GetKey(SDL_SCANCODE_9) == KeyState::DOWN)
	{
		DeathTouch();
	}

	if (input->GetKey(SDL_SCANCODE_0) == KeyState::DOWN)
	{
		Teleport();
		playerOnLocation = false;
	}

	//TELEPORT MOVEMENT
	if (!playerOnLocation)
	{
		for (const ComponentTransform* debugPointTransform : debugPoints)
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
		LOG_INFO("DEBUG MODE ACTIVATED");
	}

	else
	{
		isDebugModeActive = false;
		LOG_INFO("DEBUG MODE DEACTIVATED");
	}
}

void DebugGame::GodCamera() const 
{
	ModuleCamera* camera = App->GetModule<ModuleCamera>();

	if (!playerMoveScript->IsParalyzed() && playerJumpScript->CanJump())
	{
		playerMoveScript->SetIsParalyzed(true);
		playerJumpScript->SetCanJump(false);
		
		camera->SetSelectedPosition(1);
		camera->SetSelectedCamera(camera->GetSelectedPosition());
		LOG_INFO("GOD CAMERA ACTIVATED");
	}

	else if (playerMoveScript->IsParalyzed() && !playerJumpScript->CanJump())
	{
		playerMoveScript->SetIsParalyzed(false);
		playerJumpScript->SetCanJump(true);
		
		camera->SetSelectedPosition(0);
		camera->SetSelectedCamera(camera->GetSelectedPosition());
		LOG_INFO("GOD CAMERA DEACTIVATED");
	}
}

void DebugGame::PowerUpDrop() const
{
	if (debugPowerUp != nullptr)
	{
		PowerUpLogicScript* newPowerUpLogic = debugPowerUp->GetComponent<PowerUpLogicScript>();
		ComponentTransform* ownerTransform = player->GetComponent<ComponentTransform>();

		newPowerUpLogic->ActivatePowerUp(ownerTransform->GetOwner());
	}
}

void DebugGame::FillHealth() const
{
	playerHealthSystem->HealLife(playerHealthSystem->GetMaxHealth());
	LOG_INFO("HEALTH FILLED UP");
}

void DebugGame::BeImmortal() const
{
	if (!playerHealthSystem->IsImmortal()) 
	{
		playerHealthSystem->SetIsImmortal(true);
		LOG_INFO("IMMORTALITY ACTIVATED");
	}
	else
	{
		playerHealthSystem->SetIsImmortal(false);
		LOG_INFO("IMMORTALITY DEACTIVATED");
	}
}

void DebugGame::DeathTouch() const
{
	if (!playerAttackScript->IsDeathTouched())
	{
		playerAttackScript->SetIsDeathTouched(true);
		LOG_INFO("DEATH TOUCH ACTIVATED");
	}
	else
	{
		playerAttackScript->SetIsDeathTouched(false);
		LOG_INFO("DEATH TOUCH DEACTIVATED");
	}
}

void DebugGame::Teleport() 
{
	playerRigidBody->SetIsTrigger(true);
	currentdDebugPointTransform = debugPoints[debugCurrentPosIndex];
	playerRigidBody->SetPositionTarget(currentdDebugPointTransform->GetGlobalPosition());
	debugCurrentPosIndex = (debugCurrentPosIndex + 1) % debugPoints.size();

	LOG_INFO("TELEPORTING TO {}", currentdDebugPointTransform->GetOwner()->GetName());
}

bool DebugGame::IsTeleporting() const
{
	return isDebugModeActive && !playerOnLocation;
}
