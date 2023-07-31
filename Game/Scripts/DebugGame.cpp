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
#include "../Scripts/BixAttackScript.h"
#include "../Scripts/PlayerMoveScript.h"
#include "../Scripts/PlayerJumpScript.h"
#include "../Scripts/PlayerRotationScript.h"
#include "../Scripts/PowerUpLogicScript.h"
#include "../Scripts/GameManager.h"

#include "debugdraw.h"
#include "AxoLog.h"

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

	//Immortality Start
	player = App->GetModule<ModulePlayer>()->GetPlayer();

	playerHealthSystem = player->GetComponent<HealthSystem>();
	playerAttackScript = player->GetComponent<BixAttackScript>();
	playerMoveScript = player->GetComponent<PlayerMoveScript>();
	playerJumpScript = player->GetComponent<PlayerJumpScript>();
	playerRotationScript = player->GetComponent<PlayerRotationScript>();

	//Teleport Start
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
	//KEYBOARD INPUTS
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

	//TELEPORT MOVEMENT
	if (!playerOnLocation && isDebugModeActive)
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
		LOG_VERBOSE("DEBUG MODE ACTIVATED");
	}

	else
	{
		isDebugModeActive = false;
		LOG_VERBOSE("DEBUG MODE DEACTIVATED");
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
		LOG_VERBOSE("GOD CAMERA ACTIVATED");
	}

	else if (playerMoveScript->IsParalyzed() && !playerJumpScript->CanJump())
	{
		playerMoveScript->SetIsParalyzed(false);
		playerJumpScript->SetCanJump(true);
		
		camera->SetSelectedPosition(0);
		camera->SetSelectedCamera(camera->GetSelectedPosition());
		LOG_VERBOSE("GOD CAMERA DEACTIVATED");
	}
}

void DebugGame::PowerUpDrop() const
{
	if (DebugPowerUp != nullptr)
	{
		PowerUpLogicScript* newPowerUpLogic = DebugPowerUp->GetComponent<PowerUpLogicScript>();
		ComponentTransform* ownerTransform = player->GetComponent<ComponentTransform>();

		newPowerUpLogic->ActivatePowerUp(ownerTransform->GetOwner());
	}
}

void DebugGame::FillHealth() const
{
	playerHealthSystem->HealLife(playerHealthSystem->GetMaxHealth());
	LOG_VERBOSE("Full Health");
}

void DebugGame::BeImmortal() const
{
	if (!playerHealthSystem->IsImmortal()) 
	{
		playerHealthSystem->SetIsImmortal(true);
		LOG_VERBOSE("Immortal ON");
	}

	else if (playerHealthSystem->IsImmortal())
	{
		playerHealthSystem->SetIsImmortal(false);
		LOG_VERBOSE("Immortal OFF");
	}
}

void DebugGame::DeathTouch() const
{
	if (!playerAttackScript->IsDeathTouched())
	{
		playerAttackScript->SetIsDeathTouched(true);
		LOG_VERBOSE("Death Touch ON");
	}

	else if (playerAttackScript->IsDeathTouched())
	{
		playerAttackScript->SetIsDeathTouched(false);
		LOG_VERBOSE("Death Touch OFF");
	}
}

void DebugGame::Teleport() 
{
	playerRigidBody->SetIsTrigger(true);
	currentdDebugPointTransform = debugPoints[debugCurrentPosIndex];
	playerRigidBody->SetPositionTarget(currentdDebugPointTransform->GetGlobalPosition());
	debugCurrentPosIndex = (debugCurrentPosIndex + 1) % debugPoints.size();

	LOG_VERBOSE("TELEPORTING TO %d", debugCurrentPosIndex);
}