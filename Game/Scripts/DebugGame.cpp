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
#include <Components/ComponentScript.h>
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "debugdraw.h"
#include "Application.h"

#include "GameManager.h"

REGISTERCLASS(DebugGame);


DebugGame::DebugGame() : Script()
{
	REGISTER_FIELD(setPlayer, GameObject*);
	REGISTER_FIELD(debugPoint1, GameObject*);
	REGISTER_FIELD(debugPoint2, GameObject*);
	REGISTER_FIELD(debugPoint3, GameObject*);
	REGISTER_FIELD(debugPoint4, GameObject*);
	REGISTER_FIELD(debugPoint5, GameObject*);
	//REGISTER_FIELD_WITH_ACCESSORS(debugCurrentPos, int);

}

void DebugGame::Start()
{
	//ImmortalityStart
	player = setPlayer->GetComponent<ComponentPlayer>();

	//GameManager* manager = GameManager::GetInstance();

	/*std::vector<ComponentScript*> gameObjectScripts =*/
		/*setPlayer->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT);*/

	playerHealthSystem = setPlayer->GetComponent<HealthSystem>();
	playerAttackScript = setPlayer->GetComponent<BixAttackScript>();
	playerMoveScript = setPlayer->GetComponent<PlayerMoveScript>();
	playerJumpScript = setPlayer->GetComponent<PlayerJumpScript>();
	playerRotationScript = setPlayer->GetComponent<PlayerRotationScript>();

	isDebugModeActive = false;

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

	playerRigidBody = setPlayer->GetComponent<ComponentRigidBody>();
	playerTransform = setPlayer->GetComponent<ComponentTransform>();

	currentdDebugPointTransform = debugPoints.front();

	debugCurrentPosIndex = 0;
	playerOnLocation = false;
}

void DebugGame::Update(float deltaTime)
{
	ModuleInput* input = App->GetModule<ModuleInput>();
	
	//INPUTS

	if (input->GetKey(SDL_SCANCODE_B) == KeyState::DOWN)
	{
		ActivateDebugMode();
	}

	if (input->GetKey(SDL_SCANCODE_4) == KeyState::DOWN && isDebugModeActive)
	{
		GodCamera();
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

	//TELEPORTMOV
	if(!playerOnLocation && isDebugModeActive)
	{
		for(const ComponentTransform* debugPointTransform : debugPoints)
		{
			if (playerTransform->GetGlobalPosition().Equals(debugPointTransform->GetGlobalPosition(), 1.0f))
			{
				playerRigidBody->SetIsTrigger(false);
				playerRigidBody->DisablePositionController();
				playerOnLocation = true;
				break;
			}
		}
	}
	


}

void DebugGame::ActivateDebugMode()
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

void DebugGame::GodCamera() {

	ModuleCamera* camera = App->GetModule<ModuleCamera>();

	if (!playerMoveScript->GetIsParalized() && playerJumpScript->GetCanJump() && playerRotationScript->GetCanRotate()) 
	{
		playerMoveScript->SetIsParalized(true);
		playerJumpScript->SetCanJump(false);
		playerRotationScript->SetCanRotate(false);
		camera->SetSelectedPosition(1);
		camera->SetSelectedCamera(camera->GetSelectedPosition());
		LOG_DEBUG("GOD CAMERA ACTIVATED");
	}
	else if(playerMoveScript->GetIsParalized() && !playerJumpScript->GetCanJump() && !playerRotationScript->GetCanRotate())
	{
		playerMoveScript->SetIsParalized(false);
		playerJumpScript->SetCanJump(true);
		playerRotationScript->SetCanRotate(true);
		camera->SetSelectedPosition(0);
		camera->SetSelectedCamera(camera->GetSelectedPosition());
		LOG_DEBUG("GOD CAMERA DEACTIVATED");
	}
	

}

void DebugGame::FillHealth()
{
	playerHealthSystem->HealLife(playerHealthSystem->GetMaxHealth());
	LOG_DEBUG("Full Health");
}

void DebugGame::BeImmortal()
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

void DebugGame::DeathTouch()
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





