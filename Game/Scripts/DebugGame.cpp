#include "DebugGame.h"

#include "Application.h"
#include "ModulePlayer.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

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
#include "../Scripts/ComboManager.h"

#include "debugdraw.h"
#include "AxoLog.h"

REGISTERCLASS(DebugGame);

DebugGame::DebugGame() : Script(), isDebugModeActive(false), debugCurrentPosIndex(0), playerOnLocation(true), 
	debugPowerUp(nullptr), venomitePrefab(nullptr)
{
	REGISTER_FIELD(debugPoints, std::vector<ComponentTransform*>);

	REGISTER_FIELD(isDebugModeActive, bool);
	REGISTER_FIELD(playerOnLocation, bool);

	REGISTER_FIELD(debugPowerUp, GameObject*);

	REGISTER_FIELD(venomitePrefab, GameObject*);
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
	comboSystemScript = player->GetComponent<ComboManager>();

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

	if (input->GetKey(SDL_SCANCODE_F1) == KeyState::DOWN)
	{
#ifndef ENGINE
		App->GetModule<ModuleScene>()->SetSceneToLoad("Lib/Scenes/__LEVEL1_ALFA.axolotl");
		LOG_INFO("LOADING LEVEL 1");
#endif // ENGINE
	}
	else if (input->GetKey(SDL_SCANCODE_F2) == KeyState::DOWN)
	{
#ifndef ENGINE
		App->GetModule<ModuleScene>()->SetSceneToLoad("Lib/Scenes/_LEVEL02_ALFA.axolotl");
		LOG_INFO("LOADING LEVEL 2");
#endif // ENGINE
	}
	else if (input->GetKey(SDL_SCANCODE_F3) == KeyState::DOWN)
	{
#ifndef ENGINE
		App->GetModule<ModuleScene>()->SetSceneToLoad("Lib/Scenes/Gameplay_FinalBoss.axolotl");
		LOG_INFO("LOADING BOSS FIGHT");
#endif // ENGINE
	}
	else if (input->GetKey(SDL_SCANCODE_F4) == KeyState::DOWN)
	{
#ifndef ENGINE
		App->GetModule<ModuleScene>()->SetSceneToLoad("Lib/Scenes/00_MainMenu_VS3.axolotl");
		LOG_INFO("LOADING LEVEL 2");
#endif // ENGINE
	}
	else if (input->GetKey(SDL_SCANCODE_F5) == KeyState::DOWN)
	{
		FillComboBar();
	}
	else if (input->GetKey(SDL_SCANCODE_F6) == KeyState::DOWN)
	{
		GodCamera();
	}
	else if (input->GetKey(SDL_SCANCODE_F7) == KeyState::DOWN)
	{
		PowerUpDrop();
	}
	else if (input->GetKey(SDL_SCANCODE_F8) == KeyState::DOWN)
	{
		FillHealth();
	}
	else if (input->GetKey(SDL_SCANCODE_F9) == KeyState::DOWN)
	{
		BeImmortal();
	}
	else if (input->GetKey(SDL_SCANCODE_F10) == KeyState::DOWN)
	{
		DeathTouch();
	}
	else if (input->GetKey(SDL_SCANCODE_F11) == KeyState::DOWN)
	{
		Teleport();
		playerOnLocation = false;
	}

	// TELEPORT MOVEMENT
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

void DebugGame::SpawnNewEnemy() const
{
	GameObject* newEnemy = App->GetModule<ModuleScene>()->GetLoadedScene()->
		DuplicateGameObject("Debug Venomite", venomitePrefab, venomitePrefab->GetParent());

	float3 newEnemyPosition = player->GetComponent<ComponentTransform>()->GetGlobalPosition() + 
		player->GetComponent<ComponentTransform>()->GetGlobalForward().Normalized();

	newEnemy->GetComponent<ComponentTransform>()->SetGlobalPosition(newEnemyPosition);
	LOG_INFO("SPAWNING NEW ENEMY");
}

void DebugGame::FillComboBar() const
{
	comboSystemScript->FillComboBar();
	LOG_INFO("COMBO BAR FILLED UP");
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
