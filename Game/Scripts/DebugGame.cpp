#include "DebugGame.h"

#include "Components/ComponentPlayer.h"
#include "ModuleInput.h"
#include "HealthSystem.h"
#include "BixAttackScript.h"
#include <Components/ComponentScript.h>
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "debugdraw.h"

REGISTERCLASS(DebugGame);


DebugGame::DebugGame() : Script(), setPlayer(nullptr), playerHealthSystem(nullptr), playerAttackScript(nullptr), debugPoint1(nullptr), debugPoint2(nullptr),
debugPoint3(nullptr), debugPoint4(nullptr), debugPoint5(nullptr), playerRigidBody(nullptr), playerTransform(nullptr), debugPoint1Transform(nullptr),
debugPoint2Transform(nullptr), debugPoint3Transform(nullptr), debugPoint4Transform(nullptr), debugPoint5Transform(nullptr), 
currentdDebugPointTransform(nullptr)
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
		debugPoint1Transform = static_cast<ComponentTransform*>(debugPoint1->GetComponent(ComponentType::TRANSFORM));
	}

	if (debugPoint2)
	{
		debugPoint2Transform = static_cast<ComponentTransform*>(debugPoint2->GetComponent(ComponentType::TRANSFORM));
	}

	if (debugPoint3)
	{
		debugPoint3Transform = static_cast<ComponentTransform*>(debugPoint3->GetComponent(ComponentType::TRANSFORM));
	}

	if (debugPoint4)
	{
		debugPoint4Transform = static_cast<ComponentTransform*>(debugPoint4->GetComponent(ComponentType::TRANSFORM));
	}

	if (debugPoint5)
	{
		debugPoint5Transform = static_cast<ComponentTransform*>(debugPoint5->GetComponent(ComponentType::TRANSFORM));
	}

	playerRigidBody = static_cast<ComponentRigidBody*>(setPlayer->GetComponent(ComponentType::RIGIDBODY));
	playerTransform = static_cast<ComponentTransform*>(setPlayer->GetComponent(ComponentType::TRANSFORM));

	currentdDebugPointTransform = debugPoint1Transform;

	debugCurrentPos = 0;
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
		if (playerTransform->GetGlobalPosition().Equals(debugPoint1Transform->GetGlobalPosition(), 1.0f))
		{
			playerRigidBody->SetIsTrigger(false);
			playerRigidBody->DisablePositionController();
			playerOnLocation = true;
		}

		if (playerTransform->GetGlobalPosition().Equals(debugPoint2Transform->GetGlobalPosition(), 1.0f))
		{
			playerRigidBody->SetIsTrigger(false);
			playerRigidBody->DisablePositionController();
			playerOnLocation = true;
		}

		if (playerTransform->GetGlobalPosition().Equals(debugPoint3Transform->GetGlobalPosition(), 1.0f))
		{
			playerRigidBody->SetIsTrigger(false);
			playerRigidBody->DisablePositionController();
			playerOnLocation = true;
		}

		if (playerTransform->GetGlobalPosition().Equals(debugPoint4Transform->GetGlobalPosition(), 1.0f))
		{
			playerRigidBody->SetIsTrigger(false);
			playerRigidBody->DisablePositionController();
			playerOnLocation = true;
		}

		if (playerTransform->GetGlobalPosition().Equals(debugPoint5Transform->GetGlobalPosition(), 1.0f))
		{
			playerRigidBody->SetIsTrigger(false);
			playerRigidBody->DisablePositionController();
			playerOnLocation = true;
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
	switch (debugCurrentPos)
	{
	case 0:
		//playerOnLocation = false;
		playerRigidBody->SetIsTrigger(true);
		currentdDebugPointTransform = debugPoint1Transform;
		playerRigidBody->SetPositionTarget(currentdDebugPointTransform->GetGlobalPosition());
		debugCurrentPos++;
		ENGINE_LOG("%d", debugCurrentPos);
		break;
	case 1:
		//playerOnLocation = false;
		playerRigidBody->SetIsTrigger(true);
		playerRigidBody->SetPositionTarget(currentdDebugPointTransform->GetGlobalPosition());
		currentdDebugPointTransform = debugPoint2Transform;
		debugCurrentPos++;
		ENGINE_LOG("%d", debugCurrentPos);
		break;
	case 2:
		//playerOnLocation = false;
		playerRigidBody->SetIsTrigger(true);
		playerRigidBody->SetPositionTarget(currentdDebugPointTransform->GetGlobalPosition());
		currentdDebugPointTransform = debugPoint3Transform;
		debugCurrentPos++;
		ENGINE_LOG("%d", debugCurrentPos);
		break;
	case 3:
		//playerOnLocation = false;
		playerRigidBody->SetIsTrigger(true);
		playerRigidBody->SetPositionTarget(currentdDebugPointTransform->GetGlobalPosition());
		currentdDebugPointTransform = debugPoint4Transform;
		debugCurrentPos++;
		ENGINE_LOG("%d", debugCurrentPos);
		break;
	case 4:
		//playerOnLocation = false;
		playerRigidBody->SetIsTrigger(true);
		playerRigidBody->SetPositionTarget(currentdDebugPointTransform->GetGlobalPosition());
		currentdDebugPointTransform = debugPoint5Transform;
		debugCurrentPos++;
		ENGINE_LOG("%d", debugCurrentPos);
		break;

	case 5:
		//playerOnLocation = false;
		playerRigidBody->SetIsTrigger(true);
		playerRigidBody->SetPositionTarget(currentdDebugPointTransform->GetGlobalPosition());
		currentdDebugPointTransform = debugPoint1Transform;
		debugCurrentPos = 0;
		ENGINE_LOG("%d", debugCurrentPos);
		break;
	}
}





