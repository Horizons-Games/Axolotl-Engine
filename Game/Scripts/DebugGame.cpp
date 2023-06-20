#include "DebugGame.h"

#include "Components/ComponentPlayer.h"
#include "ModuleInput.h"
#include "HealthSystem.h"
#include "BixAttackScript.h"
#include <Components/ComponentScript.h>
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "debugdraw.h"

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
	player = static_cast<ComponentPlayer*>(setPlayer->GetComponent(ComponentType::PLAYER));

	//GameManager* manager = GameManager::GetInstance();

	std::vector<ComponentScript*> gameObjectScripts =
		setPlayer->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT);
	for (int i = 0; i < gameObjectScripts.size(); ++i)
	{
		if (gameObjectScripts[i]->GetConstructName() == "HealthSystem")
		{
			playerHealthSystem = static_cast<HealthSystem*>(gameObjectScripts[i]->GetScript());
		}
		if (gameObjectScripts[i]->GetConstructName() == "BixAttackScript")
		{
			playerAttackScript = static_cast<BixAttackScript*>(gameObjectScripts[i]->GetScript());
		}
	}
	//TeleportStart

	if (debugPoint1)
	{
		debugPoints.push_back(static_cast<ComponentTransform*>(debugPoint1->GetComponent(ComponentType::TRANSFORM)));
	}

	if (debugPoint2)
	{
		debugPoints.push_back(static_cast<ComponentTransform*>(debugPoint2->GetComponent(ComponentType::TRANSFORM)));
	}

	if (debugPoint3)
	{
		debugPoints.push_back(static_cast<ComponentTransform*>(debugPoint3->GetComponent(ComponentType::TRANSFORM)));
	}

	if (debugPoint4)
	{
		debugPoints.push_back(static_cast<ComponentTransform*>(debugPoint4->GetComponent(ComponentType::TRANSFORM)));
	}

	if (debugPoint5)
	{
		debugPoints.push_back(static_cast<ComponentTransform*>(debugPoint5->GetComponent(ComponentType::TRANSFORM)));
	}

	playerRigidBody = static_cast<ComponentRigidBody*>(setPlayer->GetComponent(ComponentType::RIGIDBODY));
	playerTransform = static_cast<ComponentTransform*>(setPlayer->GetComponent(ComponentType::TRANSFORM));

	currentdDebugPointTransform = debugPoints.front();

	debugCurrentPosIndex = 0;
	playerOnLocation = false;
}

void DebugGame::Update(float deltaTime)
{
	ModuleInput* input = App->GetModule<ModuleInput>();
	
	//INPUTS
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

	//TELEPORTMOV
	if(!playerOnLocation)
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

void DebugGame::FillHealth()
{
	playerHealthSystem->HealLife(playerHealthSystem->GetMaxHealth());
	ENGINE_LOG("Full Health");
}

void DebugGame::BeImmortal()
{
	if (!playerHealthSystem->GetIsImmortal()) 
	{
		playerHealthSystem->SetIsImmortal(true);
		ENGINE_LOG("Immortal ON");
	}
	else if (playerHealthSystem->GetIsImmortal())
	{
		playerHealthSystem->SetIsImmortal(false);
		ENGINE_LOG("Immortal OFF");
	}
}

void DebugGame::DeathTouch()
{

	if (!playerAttackScript->GetIsDeathTouched())
	{
		playerAttackScript->SetIsDeathTouched(true);
		ENGINE_LOG("Death Touch ON");
	}
	else if (playerAttackScript->GetIsDeathTouched())
	{
		playerAttackScript->SetIsDeathTouched(false);
		ENGINE_LOG("Death Touch OFF");	
	}

}

void DebugGame::Teleport()
{
	ENGINE_LOG("TELEPORTING");
	playerRigidBody->SetIsTrigger(true);
	currentdDebugPointTransform = debugPoints[debugCurrentPosIndex];
	playerRigidBody->SetPositionTarget(currentdDebugPointTransform->GetGlobalPosition());
	debugCurrentPosIndex = (debugCurrentPosIndex + 1) % debugPoints.size();
	ENGINE_LOG("%d", debugCurrentPosIndex);
}





