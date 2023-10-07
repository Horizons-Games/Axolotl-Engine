#include "StdAfx.h"
#include "PlayerHackingUseScript.h"

#include "HackZoneScript.h"
#include "UIHackingManager.h"
#include "PlayerManagerScript.h"
#include "PlayerJumpScript.h"
#include "PlayerMoveScript.h"
#include "PlayerAttackScript.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "Physics/Physics.h"

#include "MathGeoLib/Include/Geometry/Ray.h"

REGISTERCLASS(PlayerHackingUseScript);

PlayerHackingUseScript::PlayerHackingUseScript()
	: Script(), isHackingActive(false), hackingTag("Hackeable")
{
	REGISTER_FIELD(hackingManager, UIHackingManager*);
}

void PlayerHackingUseScript::Start()
{
	input = App->GetModule<ModuleInput>();
	transform = GetOwner()->GetComponent<ComponentTransform>();
	rigidBody = GetOwner()->GetComponent<ComponentRigidBody>();
	hackZone = nullptr;
	playerManager = GetOwner()->GetComponent<PlayerManagerScript>();
	isHackingButtonPressed = false;
}


void PlayerHackingUseScript::Update(float deltaTime)
{

	currentTime += deltaTime;

	PlayerActions currentAction = playerManager->GetPlayerState();
	bool isJumping = currentAction == PlayerActions::JUMPING ||
		currentAction == PlayerActions::DOUBLEJUMPING;

	bool isAttacking = playerManager->GetAttackManager()->IsInAttackAnimation();
		
	if (input->GetKey(SDL_SCANCODE_E) == KeyState::DOWN && !isHackingActive && !isJumping && !isAttacking)
	{
		FindHackZone(hackingTag);
		if (hackZone && !hackZone->IsCompleted() && !playerManager->IsParalyzed())
		{
			InitHack();
			isHackingButtonPressed = true;
		}
	}

	if (isHackingActive)
	{
		if (input->GetKey(SDL_SCANCODE_E) == KeyState::UP)
		{
			isHackingButtonPressed = false;
		}

		if (input->GetKey(SDL_SCANCODE_E) == KeyState::DOWN && !isHackingButtonPressed)
		{
			FinishHack();
		}

		if (currentTime > maxHackTime)
		{
			RestartHack();
		}
		else
		{
			SDL_Scancode key;
			SDL_GameControllerButton button;
			for (auto command : HackingCommand::allHackingCommands)
			{
				auto keyButtonPair = HackingCommand::FromCommand(command);
				key = keyButtonPair->first;
				button = keyButtonPair->second;

				if (input->GetKey(key) == KeyState::DOWN || input->GetGamepadButton(button) == KeyState::DOWN)
				{
					userCommandInputs.push_back(command);
					LOG_DEBUG("User add key/button to combination");

					hackingManager->RemoveInputVisuals();
					break;
				}
			}

			bool isMismatch = false;
			for (int i = 0; i < userCommandInputs.size(); ++i)
			{
				if (userCommandInputs[i] != commandCombination[i])
				{
					isMismatch = true;
					break;
				}
			}

			if (isMismatch)
			{
				LOG_DEBUG("Mismatch detected. Hacking will fail.");
				RestartHack();
			}

			if (userCommandInputs == commandCombination)
			{
				LOG_DEBUG("Hacking completed");
				hackZone->SetCompleted();
				FinishHack();
			}

		}
	}
}

//DEBUG METHOD
void PlayerHackingUseScript::PrintCombination()
{
	std::string combination;
	for (auto element :commandCombination)
	{
		char c = '\0';
		switch (element) {
		case COMMAND_A: 
			c = '_'; 
			break;
		case COMMAND_X:
			c = 'R'; 
			break;
		case COMMAND_Y:
			c = 'T';
			break;
		default: 
			break;
		}
		combination += c;
		
	}

	LOG_DEBUG(combination);
}

void PlayerHackingUseScript::InitHack()
{
	DisableAllInteractions();
	isHackingActive = true;
	currentTime = App->GetDeltaTime();
	maxHackTime = hackZone->GetMaxTime();
	hackZone->GenerateCombination();

	userCommandInputs.reserve(static_cast<size_t>(hackZone->GetSequenceSize()));

	commandCombination = hackZone->GetCommandCombination();
	for (auto command : commandCombination)
	{
		hackingManager->AddInputVisuals(command);
	}

	PrintCombination();
	LOG_DEBUG("Hacking is active");
}

void PlayerHackingUseScript::FinishHack()
{
	EnableAllInteractions();
	isHackingActive = false;
	hackZone = nullptr;

	userCommandInputs.clear();

	hackingManager->CleanInputVisuals();

	LOG_DEBUG("Hacking is finished");
}

void PlayerHackingUseScript::RestartHack()
{
	userCommandInputs.clear();
	hackingManager->CleanInputVisuals();

	currentTime = App->GetDeltaTime();
	maxHackTime = hackZone->GetMaxTime();
	hackZone->GenerateCombination();

	userCommandInputs.reserve(static_cast<size_t>(hackZone->GetSequenceSize()));

	commandCombination = hackZone->GetCommandCombination();
	for (auto command : commandCombination)
	{
		hackingManager->AddInputVisuals(command);
	}

	PrintCombination();
	input->Rumble();
	LOG_DEBUG("Hacking is restarted");
}

void PlayerHackingUseScript::DisableAllInteractions()
{
	playerManager->SetPlayerState(PlayerActions::IDLE);
	playerManager->PausePlayer(true);
}

void PlayerHackingUseScript::EnableAllInteractions()
{
	playerManager->SetPlayerState(PlayerActions::IDLE);
	playerManager->PausePlayer(false);
}

void PlayerHackingUseScript::FindHackZone(const std::string& tag)
{
	RaycastHit hit;
	btVector3 rigidBodyOrigin = rigidBody->GetRigidBodyOrigin();
	float3 origin = float3(rigidBodyOrigin.getX(), rigidBodyOrigin.getY(), rigidBodyOrigin.getZ());
	int raytries = 0;

	while (!hackZone && raytries < 4)
	{
		Ray ray(origin + float3(0.f, static_cast<float>(1 * raytries), 0.f), transform->GetGlobalForward());
		LineSegment line(ray, 300);
		raytries++;

		if (Physics::RaycastToTag(line, hit, owner, tag))
		{
			GameObject* hackZoneObject = hit.gameObject;
			HackZoneScript* hackZoneScript = hackZoneObject->GetComponent<HackZoneScript>();
			ComponentTransform* hackZoneTransform = hackZoneObject->GetComponent<ComponentTransform>();

			float3 hackZonePosition = hackZoneTransform->GetGlobalPosition();
			float3 playerPosition = transform->GetGlobalPosition();

			float distance = (playerPosition - hackZonePosition).Length();

			if (distance < hackZoneScript->GetInfluenceRadius())
			{
				hackZone = hackZoneScript;
			}

		}
	}
}
